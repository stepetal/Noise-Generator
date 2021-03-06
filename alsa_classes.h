/* alsa_classes.h
 *
 * Description: 	Program for noise and sinewave generation.
 *					Alsa library for sound and Xlib for graphics programing are used
 * Sources: 		1) "Introduction to sound programming with ALSA", Jeff Tranter
 *					2) Nairobi-embedded.org. ALSA and Common-off-the-shelf H/W Infrastructure
 *			   						 		 for DSP
 *					3) "Fundamentals of Xlib programming by Examples", Ross Maloney
 * Author: 			Alexandr Stpanov
 * Date: 			11.05.2017
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
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
//#include "x11_classes.h"/* for compilation without noise_gen_gui program */

#define PERIOD_FRAMES	128	



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
		int format_bits;/* bits in ALSA format */
		unsigned int max_value;/* max value correspond to ALSA format */
		int counter;/* seconds counter */
		int freq;/* sine frequency */
		int noise_type;/* type of noise for playback. 
						   Usual noise is 1, Voice-like noise is 0 
						 */
		int domain_type;/* 1 - time domain, 0 - frequency domain */
		int play_sine;/* 1 - sine is playing, 0 - sine is not playing */
		TimePanel *time_panel;/* for recording */
		ProgressBar *progress_bar;/* for playback */
		Canvas *canvas;/* for plotting */
		Button *button_reset;/* for drawing text on reset button */
		Button *button_close;/* for drawing text on close button */
		char const *name_shm;/* name of shared memory */
		int size_shm;/* size of shared memory */
		int fd_shm;/* file descriptor of shared memory */
		short int *base_shm;/* start of shared memory */
		short int *ptr_shm;/* copy of the base_shm */
		int loops;/* number of playback/capture cycles */
		/* for multithreading */
		sem_t full,empty;
		pthread_mutex_t mutex;
		/* methods */
		void set_hw_params();/* setting hardware parameters for audio */
	public:
		AlsaStream();
		~AlsaStream();
		void SetCloseButton(Button *btn_c){button_close = btn_c; }
		void SetResetButton(Button *btn_r){button_reset = btn_r; }
		void ResetTimer(){ counter = 1;}
		void SetTimePanel(TimePanel *tp){ time_panel = tp; }
		void SetProgressBar(ProgressBar *prb){ progress_bar = prb; }
		void SetCanvas(Canvas *cnv){ canvas = cnv; }
		void ShmInit();/* initializing of shm */
		/* for multithreading */
		void SemaphoresInit();/* Initializing of semaphores */

};

class AlsaPlayback: public AlsaStream{
	protected:
		void PlayUsualNoiseOrSine();
		void GenerateNoise();/* generate random data and write to shared memory */
		void GenerateSine(double *_phase);/* generate sine with certain frequency */
		void PlayVoiceLikeNoise();/* playback of noise similar to recorded voice */
		void GenerateVoiceLikeNoise();/* get voice-like noise data */

	public:
		void SetNoiseType(int n_type){ noise_type = n_type; }
		void SetDomainType(int d_type){ domain_type = d_type;}
		int GetNoiseType(){ return noise_type; }
		void PlayNoise();/* play noise according to the type */
		void SetPlaybackTime(int t){time_us = t * 1000000;}/* set playback time in seconds */
		void SetSineFreq(int f){ freq = f; }
		void PlaySine();/* sinewave playback setting */
		void PlayVoice();/* playbak of vioce just recorded */
		/* for multithreading */
		static void *ProducerWrite(void *params);/* producer write thread routine */
		static void *ConsumerRead(void *params);/* consumer read thread routine */
		void PlotNoise();/* plot noise on canvas */
};

class AlsaCapture: public AlsaStream{
	public:
		void SetCaptureTime(int t){time_us = t * 1000000;}/* set capture time in seconds */
		void CaptureVoice();/* capture voice from microphone */	
};

struct thread_struct{
	AlsaPlayback *apb_s;
};
