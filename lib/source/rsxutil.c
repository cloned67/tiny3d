#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <sysutil/video.h>
#include <rsx/gcm.h>
#include <rsx/reality.h>
#include <rsx/commands.h>

#include "rsxutil.h"

gcmContextData *context; // Context to keep track of the RSX buffer.

VideoResolution Video_Resolution; // Screen Resolution

u32 *Video_buffer[2]; // The buffer we will be drawing into
static u32 offset[2]; // The offset of the buffers in RSX memory

static u32 *depth_buffer; // Depth buffer.
static u32 depth_offset;

int Video_pitch;
static int depth_pitch;

u8 Video_aspect;

// Initilize and rsx
void init_screen() {
	// Allocate a 1Mb buffer, alligned to a 1Mb boundary to be our shared IO memory with the RSX.
	void *host_addr = memalign(1024*1024, 1024*1024);
	assert(host_addr != NULL);

	// Initilise Reality, which sets up the command buffer and shared IO memory
	context = realityInit(0x10000, 1024*1024*2, host_addr); 
	assert(context != NULL);

	VideoState state;
	assert(videoGetState(0, 0, &state) == 0); // Get the state of the display
	assert(state.state == 0); // Make sure display is enabled

	// Get the current resolution
	assert(videoGetResolution(state.displayMode.resolution, &Video_Resolution) == 0);
	
	Video_pitch = 4 * Video_Resolution.width; // each pixel is 4 bytes
	depth_pitch = 4 * Video_Resolution.width; // And each value in the depth buffer is a 16 bit float

	// Configure the buffer format to xRGB
	VideoConfiguration vconfig;
	memset(&vconfig, 0, sizeof(VideoConfiguration));
	vconfig.resolution = state.displayMode.resolution;
	vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
	vconfig.pitch = Video_pitch;
    Video_aspect=vconfig.aspect=state.displayMode.aspect;

	assert(videoConfigure(0, &vconfig, NULL, 0) == 0);
	assert(videoGetState(0, 0, &state) == 0); 

	s32 buffer_size = Video_pitch * Video_Resolution.height; 
	s32 depth_buffer_size = depth_pitch * Video_Resolution.height;
	printf("buffers will be 0x%x bytes\n", buffer_size);
	
	gcmSetFlipMode(GCM_FLIP_VSYNC); // Wait for VSYNC to flip

	// Allocate two buffers for the RSX to draw to the screen (double buffering)
	Video_buffer[0] = rsxMemAlign(16, buffer_size);
	Video_buffer[1] = rsxMemAlign(16, buffer_size);
	assert(Video_buffer[0] != NULL && Video_buffer[1] != NULL);

	depth_buffer = rsxMemAlign(16, depth_buffer_size * 4);

	assert(realityAddressToOffset(Video_buffer[0], &offset[0]) == 0);
	assert(realityAddressToOffset(Video_buffer[1], &offset[1]) == 0);
	// Setup the display buffers
	assert(gcmSetDisplayBuffer(0, offset[0], Video_pitch, Video_Resolution.width, Video_Resolution.height) == 0);
	assert(gcmSetDisplayBuffer(1, offset[1], Video_pitch, Video_Resolution.width, Video_Resolution.height) == 0);

	assert(realityAddressToOffset(depth_buffer, &depth_offset) == 0);

	gcmResetFlipStatus();
	flip(1);
    waitFlip();
}

void waitFlip() { // Block the PPU thread untill the previous flip operation has finished.
	while(gcmGetFlipStatus() != 0) 
		usleep(200);
	gcmResetFlipStatus();
}

void flip(s32 buffer) {
	assert(gcmSetFlip(context, buffer) == 0);
	realityFlushBuffer(context);
	gcmSetWaitFlip(context); // Prevent the RSX from continuing until the flip has finished.
}

void setupRenderTarget(u32 currentBuffer) {
	// Set the color0 target to point at the offset of our current surface
	realitySetRenderSurface(context, REALITY_SURFACE_COLOR0, REALITY_RSX_MEMORY, 
					offset[currentBuffer], Video_pitch);

	// Setup depth buffer
	realitySetRenderSurface(context, REALITY_SURFACE_ZETA, REALITY_RSX_MEMORY, 
					depth_offset, depth_pitch);

	// Choose color0 as the render target and tell the rsx about the surface format.
	realitySelectRenderTarget(context, REALITY_TARGET_0, 
		REALITY_TARGET_FORMAT_COLOR_X8R8G8B8 | 
		/*REALITY_TARGET_FORMAT_ZETA_Z24S8*/
        REALITY_TARGET_FORMAT_ZETA_Z24S8| 
		REALITY_TARGET_FORMAT_TYPE_LINEAR,
		Video_Resolution.width, Video_Resolution.height, 0, 0);
}
