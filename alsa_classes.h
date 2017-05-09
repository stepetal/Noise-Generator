/* alsa_classes.h
 *
 * This program allows to capture and playback alsa stream
 *
 */


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
//#include "x11_classes.h"/* for compilation with noise_gen program */


class AlsaStream{
	protected:
		/* variables */
		snd_pcm_t *pcm_handle; /* alsa pcm device */
		snd_pcm_hw_params_t *hw_params;/* hardware parameters for audio */
		snd_pcm_uframes_t frames;/* frames for alsa period size */
		int numb_of_channels;/* number of audio channels */
		int dir;/* sub unit direction(?) for alsa procedures */
		short int *playback_buffer;/* for data to alsa pcm interface */
		short int *capture_buffer;/* for data from alsa pcm interface */
		unsigned int rate;/* sample rate for alsa pcm interface */
		int time_us;/* capture/playback time in microseconds */
		int ret_code;/* returned code for alsa api*/
		unsigned int period_time;/* time for alsa period in microseconds */
		int period_size;/* size of one period in bytes */
		int counter;/* seconds counter */
		int freq;/* sine frequency */
		TimePanel *time_panel;/* for recording */
		ProgressBar *progress_bar;/* for playback */
		Canvas *canvas;/* for plotting */
		char const *name_shm;/* name of shared memory */
		int size_shm;/* size of shared memory */
		int fd_shm;/* file descriptor of shared memory */
		short int *base_shm;/* start of shared memory */
		short int *ptr_shm;/* copy of the base_shm */

		/* methods */
		void set_hw_params();/* setting hardware parameters for audio */
	public:
		void SetTimerValue(int cnt){counter = cnt;}/* for using this value at least two
													   threads are needed and they must
													   be synchronized 
													 */	
		void ResetTimer(){ counter = 1;}
		void SetTimePanel(TimePanel *tp){ time_panel = tp; }
		void SetProgressBar(ProgressBar *prb){ progress_bar = prb; }
		void SetCanvas(Canvas *cnv){ canvas = cnv; }
		void ShmInit();/* initializing of shm */
		AlsaStream();
		~AlsaStream();
};

class AlsaPlayback: public AlsaStream{
	public:
		void SetPlaybackTime(int t){time_us = t * 1000000;}/* set playback time in seconds */
		void SetSineFreq(int f){ freq = f; }
		void PlayUsualNoise();/* playback of rand noise */
		void PlayVoiceLikeNoise();/* playback of noise similar to recorded voice */
		void PlayVoice();/* playbak of vioce just recorded */
		void GenerateNoise();/* generate random data and write to shared memory */
		void GenerateSine(double *_phase);/* generate sine with certain frequency */
};

class AlsaCapture: public AlsaStream{
	public:
		void SetCaptureTime(int t){time_us = t * 1000000;}/* set capture time in seconds */
		void CaptureVoice();/* capture voice from microphone */	
};


