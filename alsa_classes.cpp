/* alsa_classes.cpp
 *
 * This program allows to capture and playback alsa stream
 *
 */


#include "alsa_classes.h"

AlsaStream::AlsaStream()
{
	pcm_handle = NULL;
	hw_params = NULL;
	numb_of_channels = 1;
	rate = 44100;
	frames = 128;
	time_us = 5000000;
	dir = 0;
	ret_code = 0;
	period_time = 0;
	period_size = frames * 2;/* 2 bytes per one frame */
	counter = 1;
	time_panel = NULL;
	progress_bar = NULL;
	canvas = NULL;
	name_shm = "rand_data_shm";
	base_shm = NULL;
	ptr_shm = NULL;
	fd_shm = -1;
	loops = 0;
	freq = 440;
	format_bits = snd_pcm_format_width(SND_PCM_FORMAT_S16);
	max_value = (1 << (format_bits -1)) - 1 ;
	noise_type = 1;
	domain_type = 1;
	play_sine = 0;
}

AlsaStream::~AlsaStream()
{
	if (munmap(base_shm,size_shm) == -1){
		fprintf(stderr,"Unable to unmap: %s\n",strerror(errno));
	}
	if (close(fd_shm) == -1){
		fprintf(stderr,"Unable to close shm: %s\n",strerror(errno));
	}
}


void AlsaStream::set_hw_params()
{
	/* allocate memory for hw params*/
	snd_pcm_hw_params_alloca(&hw_params);
	/* set default values for hw params */
	snd_pcm_hw_params_any(pcm_handle,hw_params);
	/* set access type for hw buffer */
	snd_pcm_hw_params_set_access(pcm_handle,hw_params,SND_PCM_ACCESS_RW_INTERLEAVED);
	/* set format for samples */
	snd_pcm_hw_params_set_format(pcm_handle,hw_params,SND_PCM_FORMAT_S16);
	/* set number of channels */
	snd_pcm_hw_params_set_channels(pcm_handle,hw_params,numb_of_channels);
	/* set sample rate */
	snd_pcm_hw_params_set_rate_near(pcm_handle,hw_params,&rate,&dir);
	/* set alsa period size in frames */
	snd_pcm_hw_params_set_period_size_near(pcm_handle,hw_params,&frames,&dir);
	/* fix choice of parameters */
	ret_code = snd_pcm_hw_params(pcm_handle,hw_params);
	if (ret_code < 0){
		fprintf(stderr,"Unable to set params: %s\n",snd_strerror(ret_code));
		exit(EXIT_FAILURE);
	}
	/* get time in us for one alsa period */
	snd_pcm_hw_params_get_period_time(hw_params,&period_time,&dir);
	playback_buffer = (short int *)malloc(period_size * sizeof(short int));
	capture_buffer = (short int *)malloc(period_size * sizeof(short int));
}

void AlsaStream::ShmInit()
{
	fd_shm = shm_open(name_shm,O_CREAT | O_RDWR,0666);
	if (fd_shm == -1){
		fprintf(stderr,"Shared memory failed: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	size_shm = period_size;
	ftruncate(fd_shm,period_size);
	base_shm = (short int *)mmap(0,period_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd_shm,0);
	if (base_shm == MAP_FAILED){
		fprintf(stderr,"Map failed: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void AlsaStream::SemaphoresInit()
{
	pthread_mutex_init(&mutex,NULL);
	sem_init(&full,0,0);/* full semaphore is 0 therefore producer makes values first */
	sem_init(&empty,0,1);
}

/* methods of AlsaPlayback class */

void *AlsaPlayback::ConsumerRead(void *params)
{
	thread_struct *t_struct_c = (thread_struct *)params;
	t_struct_c -> apb_s -> PlotNoise();
}


void AlsaPlayback::PlotNoise()
{
	int loops_c;
	short int *ptr_thread;
	loops_c = time_us / period_size;
	if (canvas != NULL){
		while (loops_c){
			loops_c--;
			sem_wait(&full);/* wait until full will be 1 */
			pthread_mutex_lock(&mutex);/* for mutual exclusion */
			/* Draw every third period. In other case problems with sound occur */
			if(loops_c % 6  == 0){
				XLockDisplay(canvas -> GetDisplay());/* preventive measures */
				canvas -> ClearCanvas();
				ptr_thread = base_shm;
				if( domain_type){
					canvas -> PlotArray(ptr_thread);
				} else {
					canvas -> PlotArrayFFT(ptr_thread);
				}
 				XUnlockDisplay(canvas -> GetDisplay());
			}
			pthread_mutex_unlock(&mutex);
			sem_post(&empty);/* increase empty on 1 */
		}
	}
}

void *AlsaPlayback::ProducerWrite(void *params)
{
	thread_struct *t_struct_p = (thread_struct *)params;
	t_struct_p -> apb_s -> PlayNoise();
}


void AlsaPlayback::PlayNoise()
{
	if(noise_type){
		PlayUsualNoiseOrSine();
	} else {
		fprintf(stderr,"Play voice-like noise\n");
		PlayVoiceLikeNoise();
	}
}

void AlsaPlayback::GenerateNoise()
{
	int i;
	ptr_shm = base_shm;
	for (i = 0;i < period_size;i++){
		ptr_shm[i] = (short int)rand() % max_value;
		playback_buffer[i] = ptr_shm[i];
	}
}

void AlsaPlayback::GenerateVoiceLikeNoise()
{
	int i;
	ptr_shm = base_shm;
	for (i = 0;i < period_size;i++){
		ptr_shm[i] = playback_buffer[i];
	}
}

void AlsaPlayback::GenerateSine(double *_phase)
{
	int i;
	int j;
	double phase = *_phase;
	double max_phase = 2. * M_PI;
	double step = ((max_phase) * freq) / (double)rate; 
	int bps = format_bits / 8;
	int res;
	ptr_shm = base_shm;
	for (i = 0;i < period_size;i++){
		for (j = 0;j < bps;j++){
			ptr_shm[i] = (short int)(sin(phase) * max_value);
		}
		phase += step;
		if (phase >= max_phase)
			phase -= max_phase;
	}
	*_phase = phase;
}

void AlsaPlayback::PlaySine()
{
	play_sine = 1;
}


void AlsaPlayback::PlayUsualNoiseOrSine()
{
	int i;
	/* number of playback cycles per second */
	int loops_ps;
	/* for sinewave generator */
	double phase = 0;
	/* open default pcm device for playback */
	ret_code = snd_pcm_open(&pcm_handle,"default",SND_PCM_STREAM_PLAYBACK,0);
	srand(time(NULL));
	if (ret_code < 0){
		fprintf(stderr,"Unable to open pcm device: %s\n",snd_strerror(ret_code));
		exit(EXIT_FAILURE);
	}
	set_hw_params();
	loops = time_us / period_time;
	loops_ps = 1000000 / period_time;
	while (loops > 0){
		loops--;
		if (!(play_sine)){
			sem_wait(&empty);/* firstly empty semaphore is 1 therefore producer can 
							    generate period_size items
							  */
			pthread_mutex_lock(&mutex);
			/* Change value of progress bar and timer panel every second */
			if( (!(loops % loops_ps)) && (loops != 0)){
				if (progress_bar != NULL){
					progress_bar -> CheckForIncrease(counter);
					progress_bar -> Draw();
				}
				if (time_panel != NULL){
					time_panel -> RenewCounter(counter);	
					time_panel -> Draw();
				}
				if (button_close != NULL){
					button_close -> ButtonReleasedState();
					button_close -> DrawText();
				}
				if (button_reset != NULL){
					button_reset -> ButtonReleasedState();
					button_reset -> DrawText();
				}
				counter++;
			}
			GenerateNoise();
			pthread_mutex_unlock(&mutex);
			sem_post(&full);
		} else {
			sem_wait(&empty);
			pthread_mutex_lock(&mutex);
			/* Change value of progress bar and timer panel every second */
			if( (!(loops % loops_ps)) && (loops != 0)){
				if (progress_bar != NULL){
					progress_bar -> CheckForIncrease(counter);
					progress_bar -> Draw();
				}
				if (time_panel != NULL){
					time_panel -> RenewCounter(counter);	
					time_panel -> Draw();
				}
				if (button_close != NULL){
					button_close -> ButtonReleasedState();
					button_close -> DrawText();
				}
				if (button_reset != NULL){
					button_reset -> ButtonReleasedState();
					button_reset -> DrawText();
				}
				counter++;
			}
			GenerateSine(&phase);
			pthread_mutex_unlock(&mutex);
			sem_post(&full);
		}
		ret_code = snd_pcm_writei(pcm_handle,playback_buffer,frames);
		if (ret_code == -EPIPE){
			snd_pcm_prepare(pcm_handle);
		} else if (ret_code < 0){
			fprintf(stderr,"error from writei: %s\n",snd_strerror(ret_code));
		} else if (ret_code != (int)frames){
			fprintf(stderr,"short write: must be writeen %i, wrote %i\n",(int)frames,ret_code);
		}
	}
	if (play_sine){
		play_sine = 0;
	}
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(playback_buffer);
	free(capture_buffer);
	if (time_panel != NULL){
		time_panel -> Stop();
		time_panel -> SetTopValue(time_us / 1000000);
		time_panel -> Draw();
	}
	if (progress_bar != NULL){
		progress_bar -> ResetBar();
		progress_bar -> Draw();
	}
	if (canvas != NULL){
		canvas -> ClearCanvas();
	}
	ResetTimer();
	sleep(5);/* delay before next sound playback */
}

void AlsaPlayback::PlayVoice()
{
	int i;
	/* number of playback cycles per second */
	int loops_ps;
	/* file with voice */
	FILE *voice_file;
	voice_file = fopen("voice_file.raw","r");
	if (voice_file == NULL){
		fprintf(stderr,"Unable to open file: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	ret_code = snd_pcm_open(&pcm_handle,"default",SND_PCM_STREAM_PLAYBACK,0);
	if (ret_code < 0){
		fprintf(stderr,"Unable to open pcm device: %s\n",snd_strerror(ret_code));
		exit(EXIT_SUCCESS);
	}
	set_hw_params();
	loops = time_us / period_time;
	loops_ps = 1000000 / period_time;
	while (loops > 0){
		loops--;
		
		if( (!(loops % loops_ps)) && (loops != 0)){
			fprintf(stderr,"%i\n",counter);
			(counter)++;
		}
		
		if(!feof(voice_file)){
			fread(playback_buffer,1,period_size,voice_file);
			ret_code = snd_pcm_writei(pcm_handle,playback_buffer,frames);
			if (ret_code == -EPIPE){
				snd_pcm_prepare(pcm_handle);
			} else if (ret_code < 0){
				fprintf(stderr,"error from writei: %s\n",snd_strerror(ret_code));
			} else if (ret_code != (int)frames){
				fprintf(stderr,"short write: must be writeen %i, wrote %i\n",(int)frames,ret_code);
			}
		}

	}
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	fclose(voice_file);
	free(playback_buffer);
	free(capture_buffer);
	sleep(5);/* delay before next sound playback */
}

void  AlsaPlayback::PlayVoiceLikeNoise()
{
	int i;
	/* number of playback cycles per second */
	int loops_ps;
	/* offset for file */
	long offset;
	/* file for raw voice_like noise data */
	FILE *voice_like_noise_file;
	voice_like_noise_file = fopen("voice_like_noise_file.raw","r");
	if (voice_like_noise_file == NULL){
		fprintf(stderr,"Unable to open file: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	ret_code = snd_pcm_open(&pcm_handle,"default",SND_PCM_STREAM_PLAYBACK,0);
	if (ret_code < 0){
		fprintf(stderr,"Unable to open pcm device: %s\n",snd_strerror(ret_code));
		exit(EXIT_SUCCESS);
	}
	set_hw_params();
	loops = time_us / period_time;
	loops_ps = 1000000 / period_time;
	while (loops > 0){
		loops--;

		if(!feof(voice_like_noise_file)){
			fread(playback_buffer,1,period_size,voice_like_noise_file);
			sem_wait(&empty);/* firstly empty semaphore is 1 therefore producer can 
							    generate period_size items
							  */
			pthread_mutex_lock(&mutex);
			/* Change value of progress bar and timer panel every second */
			if( (!(loops % loops_ps)) && (loops != 0)){
				if (progress_bar != NULL){
					progress_bar -> CheckForIncrease(counter);
					progress_bar -> Draw();
				}
				if (time_panel != NULL){
					time_panel -> RenewCounter(counter);	
					time_panel -> Draw();
				}
				if (button_close != NULL){
					button_close -> ButtonReleasedState();
					button_close -> DrawText();
				}
				if (button_reset != NULL){
					button_reset -> ButtonReleasedState();
					button_reset -> DrawText();
				}
				counter++;
			}
			GenerateVoiceLikeNoise();
			pthread_mutex_unlock(&mutex);
			sem_post(&full);
			ret_code = snd_pcm_writei(pcm_handle,playback_buffer,frames);
			if (ret_code == -EPIPE){
				snd_pcm_prepare(pcm_handle);
			} else if (ret_code < 0){
				fprintf(stderr,"error from writei: %s\n",snd_strerror(ret_code));
			} else if (ret_code != (int)frames){
				fprintf(stderr,"short write: must be writeen %i, wrote %i\n",(int)frames,ret_code);
			}
		}
	}
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	fclose(voice_like_noise_file);
	free(playback_buffer);
	free(capture_buffer);
	if (time_panel != NULL){
		time_panel -> Stop();
		time_panel -> SetTopValue(time_us / 1000000);
		time_panel -> Draw();
	}
	if (progress_bar != NULL){
		progress_bar -> ResetBar();
		progress_bar -> Draw();
	}
	if (canvas != NULL){
		canvas -> ClearCanvas();
	}
	sleep(5);/* delay before next sound playback */
}



void AlsaCapture::CaptureVoice()
{

	int i;
	int tmp;
	int tmp2;
	int tmp3;
	/* number of capture cycles per second */
	int loops_ps;
	/* file for raw capture data */
	FILE *voice_file;
	/* file for voice-like noise */
	FILE *voice_like_noise_file;
	voice_file = fopen("voice_file.raw","w");
	if (voice_file == NULL){
		fprintf(stderr,"Unable to open file: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	voice_like_noise_file = fopen("voice_like_noise_file.raw","w");
	if (voice_file == NULL){
		fprintf(stderr,"Unable to open file: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	ret_code = snd_pcm_open(&pcm_handle,"default",SND_PCM_STREAM_CAPTURE,0);
	if (ret_code < 0){
		fprintf(stderr,"Unable to open pcm device: %s\n",snd_strerror(ret_code));
		exit(EXIT_SUCCESS);
	}
	set_hw_params();
	srand(time(NULL));
	loops = time_us / period_time; 
	loops_ps = 1000000 / period_time;
	while (loops > 0){
		loops--;
		if( (!(loops % loops_ps)) && (loops != 0)){
			if (time_panel != NULL){
				time_panel -> RenewCounter(counter);	
				time_panel -> Draw();
			}
			(counter)++;
		}
	
		ret_code = snd_pcm_readi(pcm_handle,capture_buffer,frames);
		if (ret_code == -EPIPE){
			fprintf(stderr,"Overrun occured...\n");
			snd_pcm_prepare(pcm_handle);
		} else if (ret_code < 0){
			fprintf(stderr,"Error from readi: %s",snd_strerror(ret_code));
		} else if (ret_code != (int)frames){
			fprintf(stderr,"Short read, must be read: %i, read only: %i\n",(int)frames,ret_code);
		}
//		/* write voice-like noise to sharad memory */
	//	ptr_shm = base_shm;
		for (i = 0;i < period_size;i++){
			tmp = rand()%period_size;
			if ((capture_buffer[tmp] < max_value - 1500) ||
			     capture_buffer[tmp] > -1 * max_value + 1500){
				tmp2 = rand()%1500;	 
				if(capture_buffer[tmp] > 0){
					tmp3 = capture_buffer[tmp] - tmp2;
				} else {
					tmp3 = capture_buffer[tmp] + tmp2;
				}
			} else {
				tmp3 = capture_buffer[tmp];
			}
			//ptr_shm[i] = tmp3;
			playback_buffer[i] = tmp3;
		}
		fwrite(playback_buffer,1,period_size,voice_like_noise_file);
		fwrite(capture_buffer,1,period_size,voice_file);
	}
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	fclose(voice_file);
	fclose(voice_like_noise_file);
	free(playback_buffer);
	free(capture_buffer);
}
