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
	freq = 440;
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

void AlsaPlayback::GenerateNoise()
{
	int i;
	ptr_shm = base_shm;
	for (i = 0;i < period_size;i++){
		ptr_shm[i] = (short int)rand()%65535;
	//	ptr_shm += sprintf(ptr_shm,"%c",rand()%255);
	}
}

void AlsaPlayback::GenerateSine(double *_phase)
{
	int i;
	int j;
	double phase = *_phase;
	double max_phase = 2. * M_PI;
	double step = ((max_phase) * freq) / (double)rate; 
	int format_bits = snd_pcm_format_width(SND_PCM_FORMAT_S16);
	unsigned int max_value = (1 << (format_bits -1)) - 1 ;
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


void AlsaPlayback::PlayUsualNoise()
{
	/* number of playback cycles */
	int loops;
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
		if( (!(loops % loops_ps)) && (loops != 0)){
			if (progress_bar != NULL){
				progress_bar -> CheckForIncrease(counter);
				progress_bar -> Draw();
			}
			if (time_panel != NULL){
				time_panel -> RenewCounter(counter);	
				time_panel -> Draw();
				(counter)++;
			}
		}
		/*
		for (i = 0;i < period_size;i++){
			playback_buffer[i] = rand()%65535;
		}
		*/
		//ret_code = snd_pcm_writei(pcm_handle,playback_buffer,frames);
		//GenerateNoise();
		GenerateSine(&phase);
		canvas -> PlotArray(base_shm);
		ret_code = snd_pcm_writei(pcm_handle,base_shm,frames);
		
		if (ret_code == -EPIPE){
			snd_pcm_prepare(pcm_handle);
		} else if (ret_code < 0){
			fprintf(stderr,"error from writei: %s\n",snd_strerror(ret_code));
		} else if (ret_code != (int)frames){
			fprintf(stderr,"short write: must be writeen %i, wrote %i\n",(int)frames,ret_code);
		}
		canvas -> ClearCanvas();
	}
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(playback_buffer);
	free(capture_buffer);
	sleep(5);/* delay before next sound playback */
}

void AlsaPlayback::PlayVoice()
{
	/* number of playback cycles */	
	int loops;
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
	/* number of playback cycles */	
	int loops;
	int i;
	/* number of playback cycles per second */
	int loops_ps;
	/* offset for file */
	long offset;
	/* file with voice-like noise */
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
		
		if( (!(loops % loops_ps)) && (loops != 0)){
			(counter)++;
		}
		
		if(!feof(voice_like_noise_file)){
			fread(playback_buffer,1,period_size,voice_like_noise_file);
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
	sleep(5);/* delay before next sound playback */
}



void AlsaCapture::CaptureVoice()
{

	/* number of capture cycles */
	int loops;
	/* number of capture cycles per second */
	int loops_ps;
	int i;
	/* file for raw capture data */
	FILE *voice_file;
	/* file for voice-like noise data */
	FILE *voice_like_noise_file;
	voice_file = fopen("voice_file.raw","w");
	if (voice_file == NULL){
		fprintf(stderr,"Unable to open file: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	voice_like_noise_file = fopen("voice_like_noise_file.raw","w");
	if (voice_like_noise_file == NULL){
		fprintf(stderr,"Unable to open file: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	ret_code = snd_pcm_open(&pcm_handle,"default",SND_PCM_STREAM_CAPTURE,0);
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
		/* use playback_buffer for storing voice-like noise data */
		for (i = 0;i < frames;i++){
			playback_buffer[i] = capture_buffer[rand()%frames]; 
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
