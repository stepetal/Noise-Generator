/* main.cpp
 *
 * Implementation of widgets in x11
 * 
 */

#include "x11_classes.h"
#include "alsa_classes.h"

/* constants for playback and capture */
#define USUAL_NOISE						1
#define VOICE_LIKE_NOISE				0

/* constants for MainWindow */
#define MAIN_WINDOW_NUMBER				3	/* number of MainWindows */
#define MAIN_WINDOW_WIDTH				650
#define MAIN_WINDOW_HEIGHT				530

/* constants for big Panels */
#define BIG_PANEL_NUMBER 				9	/* number of big Panels */	
#define BIG_PANEL_WIDTH					140
#define BIG_PANEL_HEIGHT				130
#define BIG_PANEL_BORDER_WIDTH 			2
/* constants for CheckBox Panels */
#define CHECK_BOX_PANEL_NUMBER			11	/* number of CheckBox Panels */
#define CHECK_BOX_PANEL_WIDTH			110
#define CHECK_BOX_PANEL_HEIGHT			20
#define CHECK_BOX_PANEL_BORDER_WIDTH 	1
/* constants for EditBox Panels */
#define EDIT_BOX_PANEL_NUMBER 			1	/* number of EditBox Panels */
#define EDIT_BOX_PANEL_WIDTH			80	
#define EDIT_BOX_PANEL_HEIGHT			50
#define EDIT_BOX_PANEL_BORDER_WIDTH		1
/* constants for CheckBoxes */
#define CHECK_BOX_NUMBER 				11	/* number of CheckBoxes */
#define CHECK_BOX_WIDTH					16
#define CHECK_BOX_HEIGHT				16
#define CHECK_BOX_BORDER_WIDTH			1
/* constants for Buttons */ 
#define BUTTON_NUMBER					15 	/* number of Buttons */
#define BUTTON_WIDTH					60	
#define BUTTON_HEIGHT					20
#define BUTTON_BORDER_WIDTH				1
/* constants for EditBox */
#define EDIT_BOX_NUMBER					1	/* number of EditBoxes*/
#define EDIT_BOX_WIDTH					60 
#define EDIT_BOX_HEIGHT					20
#define EDIT_BOX_BORDER_WIDTH			1
/* constants for ProgressBar */
#define PROGRESS_BAR_NUMBER				1	/* number of ProgressBar units*/
#define PROGRESS_BAR_WIDTH				300
#define PROGRESS_BAR_HEIGHT				60
#define PROGRESS_BAR_BORDER_WIDTH		2
/* constants for TimePanel */
#define TIME_PANEL_NUMBER				2	/* number of TimePanels */
#define TIME_PANEL_WIDTH				100
#define TIME_PANEL_HEIGHT				50
#define TIME_PANEL_BORDER_WIDTH			1
/* constants for Canvas */
#define CANVAS_NUMBER					1
#define CANVAS_WIDTH					500
#define CANVAS_HEIGHT 					300
#define CANVAS_BORDER_WIDTH				1

Display *display;
int screen_number;

void CreateDisplay()
{
	
	display = XOpenDisplay(NULL);
	if (display == NULL){
		fprintf(stderr,"Unable to open the display\n");
		exit(EXIT_FAILURE);
	}
	screen_number = DefaultScreen(display);
}
int main(int argc,char *argv[])
{
	int i,j;
	char ch;/* char form keyboard */
	KeySym key_sym;/* KeySym for key */
	bool done = false;/* condition for ending cycle */
	int number_of_clicks = 120;
	int counter = 1;
	XEvent event;/* event to be catched */
	int freq_array[4] = {344,440,688,700};/* available sine frequences */
	int time_array[3] = {5,10,20};/* available recording and playback time */
	/* options for sound processing */
	bool time_domain = true;
	int sine_freq = freq_array[3];
	int time = time_array[1];
	/* temp data for canvas */
	int data[10] = {150,140,130,20,50,171,183,139,43,100};
	int data_length = 10;
	/* alsa data for plotting */
	int frames = 128 * 2;/* 2 bytes per one frame */
	int frame_length = 32765;/* max value for short int */

	/* options for windows*/
	unsigned int main_window_width;/* for manual changing of main window width */
	unsigned int main_window_height;/* for mainual changing of main window height */
	unsigned int big_panel_width;/* for manual changing of panel width */
	unsigned int big_panel_height;/* for manual changing of panel height */
	unsigned int big_panel_text_x;/* x coordinate for big panel */
	unsigned int big_panel_text_y;/* y coordinate for big panel */
	char const *main_window_color = "green";
	char const *button_color = "gray";
	char const *big_panel_color = "dim gray";
	char const *check_box_color = "white";
	char const *check_box_panel_color = "slate gray";
	char const *edit_box_panel_color = "slate gray";
	char const *edit_box_color = "white";
	char const *progress_bar_color = "dim gray";
	char const *time_panel_color = "slate gray";
	char const *canvas_color = "white smoke";
	char const *font_name = "lucidasanstypewriter-10";
	char const *font_color_name = "black";
	char const *text = "Text in main window";
	/* object definition */
	/* for GUI */
	Panel *big_panel_array[BIG_PANEL_NUMBER];
	Panel *check_box_panel_array[CHECK_BOX_PANEL_NUMBER];
	Panel *edit_box_panel_array[EDIT_BOX_PANEL_NUMBER];
	Button *button_array[BUTTON_NUMBER];
	CheckBox *check_box_array[CHECK_BOX_NUMBER];
	MainWindow *main_window_array[MAIN_WINDOW_NUMBER];
	EditBox *edit_box_array[EDIT_BOX_NUMBER];
	ProgressBar *progress_bar_array[PROGRESS_BAR_NUMBER];
	TimePanel *time_panel_array[TIME_PANEL_NUMBER];
	Canvas *canvas_array[CANVAS_NUMBER];
	/* for playback and capture */
	AlsaPlayback *apb;
	AlsaCapture *acp;
	/* Setting of parameters */
	/* for big Panels */
	char const *big_panel_text[BIG_PANEL_NUMBER] = {"Domain","Noise","Sine wave","Recording",
													"Time"," "," "," ","Noise Generator"};
	int big_panel_x_array[BIG_PANEL_NUMBER] = {20,20,20,280,180,30,30,250,180};
	int big_panel_y_array[BIG_PANEL_NUMBER] = {70,160,310,385,385,260,350,400,70};
	Window big_panel_parent[BIG_PANEL_NUMBER];
	/* for EditBox Panels */
	char const *edit_box_panel_text[EDIT_BOX_PANEL_NUMBER] = {"Value,Hz"};
	int edit_box_panel_x_array[EDIT_BOX_PANEL_NUMBER] = {30};
	int edit_box_panel_y_array[EDIT_BOX_PANEL_NUMBER] = {60};
	Window edit_box_panel_parent[EDIT_BOX_PANEL_NUMBER];
	/* for CheckBox Panels */
	char const *check_box_panel_text[CHECK_BOX_PANEL_NUMBER] = {"Frequency","Time","Usual","Voice-like",
																"344 Hz","440 Hz","688 Hz","700 Hz","5 sec",
																"10 sec","20 sec"};
	int check_box_panel_x_array[CHECK_BOX_PANEL_NUMBER] = {15,15,15,15,15,15,15,15,15,15,15};
	int check_box_panel_y_array[CHECK_BOX_PANEL_NUMBER] = {15,40,30,60,5,30,55,80,10,35,60};
	Window check_box_panel_parent[CHECK_BOX_PANEL_NUMBER];
	/* for CheckBoxes */
	int check_box_x_value = 90; 
	int check_box_y_value = 1;
	Window check_box_parent[CHECK_BOX_NUMBER];
	/* for Buttons */
	char const *button_text[BUTTON_NUMBER] = {"Play","Play","Sine..","Time..",
											  "Start","OK","OK","Done","Play",
											  "Reset","Quit","About","Close","Close","Reset"};
	int button_x_value = 40;
	int button_y_array[BUTTON_NUMBER] = {90,120,30,35,25,105,105,10,35,50,5,5,170,10,50};
	Window button_parent[BUTTON_NUMBER];
	/* for MainWindows */
	int main_window_x_array[MAIN_WINDOW_NUMBER] = {50,100,150};
	int main_window_y_array[MAIN_WINDOW_NUMBER] = {50,100,100};
	char const *main_window_titles[MAIN_WINDOW_NUMBER] = {"Window","About","Canvas"};
	/* for EditBox */
	int edit_box_x_array[EDIT_BOX_NUMBER] = {10};
	int edit_box_y_array[EDIT_BOX_NUMBER] = {20};
	Window edit_box_parent[EDIT_BOX_NUMBER];
	/* for ProgressBar */
	int progress_bar_x_array[PROGRESS_BAR_NUMBER] = {150};
	int progress_bar_y_array[PROGRESS_BAR_NUMBER] = {370};
	Window progress_bar_parent[PROGRESS_BAR_NUMBER];
	/* for TimePanel */
	int time_panel_x_array[TIME_PANEL_NUMBER] = {90,50};
	int time_panel_y_array[TIME_PANEL_NUMBER] = {20,370};
	Window time_panel_parent[TIME_PANEL_NUMBER];
	/* for Canvas */
	int canvas_x_array[CANVAS_NUMBER] = {50};
	int canvas_y_array[CANVAS_NUMBER] = {50};
	Window canvas_parent[CANVAS_NUMBER];
	/* Create display for all windows */
	CreateDisplay();
	/* Create all components */
	for (i = 0;i < MAIN_WINDOW_NUMBER;i++){
		main_window_width = MAIN_WINDOW_WIDTH;
		main_window_height = MAIN_WINDOW_HEIGHT;
		if (i == 1){
			main_window_width = 200;
			main_window_height = 200;
		}
		if (i == 2){
			main_window_width = 600;
			main_window_height = 460;
		}
		main_window_array[i] = new MainWindow(main_window_width,main_window_height,
											  main_window_x_array[i],
											  main_window_y_array[i],
											  main_window_color,
											  main_window_titles[i],
											  font_name,font_color_name,
											  display,screen_number);
		if (i == 0)
			main_window_array[i] -> ShowWindow();
	}
	/* Create big Panels */
	for (i = 0;i < BIG_PANEL_NUMBER;i++){
		big_panel_parent[i] = main_window_array[0] -> GetWindow();
	}
	for (i = 0;i < BIG_PANEL_NUMBER;i++){ 
		big_panel_width = BIG_PANEL_WIDTH;
		big_panel_height = BIG_PANEL_HEIGHT;
		
		if (i == 0){
			big_panel_height = 70;
		}
		if (i == 4){
			big_panel_width = 80;
			big_panel_height = (BIG_PANEL_HEIGHT + 20) / 2;
		}
		if (i == 2){
			big_panel_width = BIG_PANEL_WIDTH;
			big_panel_height = BIG_PANEL_HEIGHT + 20;
		}
		if (i == 3){
			big_panel_width = PROGRESS_BAR_WIDTH / 2 + 50;
			big_panel_height = (BIG_PANEL_HEIGHT + 20) / 2;
		}
		if (i == 7){
			big_panel_width = 100;
			big_panel_height = 60;
		}
		if (i == 8){
			big_panel_width = 300;
			big_panel_height = 300;
		}
		
		big_panel_array[i] = new Panel(big_panel_width,big_panel_height,
									   big_panel_x_array[i],big_panel_y_array[i],
									   big_panel_color,font_name,font_color_name,
									   big_panel_parent[i],display,screen_number,
									   BIG_PANEL_BORDER_WIDTH); 	
		big_panel_array[i] -> SetText(big_panel_text[i]);
		if (i != 7 || i != 5 || i != 6){
			big_panel_array[i] -> ShowWindow();
		}
	} 
	/* Create CheckBox Panels */
	check_box_panel_parent[0] = big_panel_array[0] -> GetWindow();
	check_box_panel_parent[1] = big_panel_array[0] -> GetWindow();
	check_box_panel_parent[2] = big_panel_array[1] -> GetWindow();
	check_box_panel_parent[3] = big_panel_array[1] -> GetWindow();
	check_box_panel_parent[4] = big_panel_array[5] -> GetWindow();
	check_box_panel_parent[5] = big_panel_array[5] -> GetWindow();
	check_box_panel_parent[6] = big_panel_array[5] -> GetWindow();
	check_box_panel_parent[7] = big_panel_array[5] -> GetWindow();
	check_box_panel_parent[8] = big_panel_array[6] -> GetWindow();
	check_box_panel_parent[9] = big_panel_array[6] -> GetWindow();
	check_box_panel_parent[10] = big_panel_array[6] -> GetWindow();

	for (i = 0;i < CHECK_BOX_PANEL_NUMBER;i++){
		check_box_panel_array[i] = new Panel(CHECK_BOX_PANEL_WIDTH,CHECK_BOX_PANEL_HEIGHT,
										 check_box_panel_x_array[i],check_box_panel_y_array[i],
										 check_box_panel_color,font_name,font_color_name,
										 check_box_panel_parent[i],display,screen_number,
										 CHECK_BOX_PANEL_BORDER_WIDTH);
		check_box_panel_array[i] -> SetText(check_box_panel_text[i]);
		if(i !=4 || i != 5 || i != 6 || 
		   i !=7 || i != 8 || i != 9 || i != 10){

			check_box_panel_array[i] -> ShowWindow();
		}

	}
	/* Create EditBox Panels */
	edit_box_panel_parent[0] = big_panel_array[2] -> GetWindow();
	for (i = 0;i < EDIT_BOX_PANEL_NUMBER;i++){
		edit_box_panel_array[i] = new Panel(EDIT_BOX_PANEL_WIDTH,EDIT_BOX_PANEL_HEIGHT,
											edit_box_panel_x_array[i],edit_box_panel_y_array[i],
											edit_box_panel_color,font_name,font_color_name,
											edit_box_panel_parent[i],display,screen_number,
											EDIT_BOX_PANEL_BORDER_WIDTH);
		edit_box_panel_array[i] -> SetText(edit_box_panel_text[i]);
		edit_box_panel_array[i] -> ShowWindow();
	}
	/* Create Buttons */
	button_parent[0] = big_panel_array[1] -> GetWindow();
	button_parent[1] = big_panel_array[2] -> GetWindow();
	button_parent[2] = big_panel_array[2] -> GetWindow();
	button_parent[3] = big_panel_array[4] -> GetWindow();
	button_parent[4] = big_panel_array[3] -> GetWindow();
	button_parent[5] = big_panel_array[5] -> GetWindow();
	button_parent[6] = big_panel_array[6] -> GetWindow();
	button_parent[7] = big_panel_array[7] -> GetWindow();
	button_parent[8] = big_panel_array[7] -> GetWindow();
	button_parent[9] = big_panel_array[3] -> GetWindow();
	button_parent[10] = main_window_array[0] -> GetWindow();
	button_parent[11] = main_window_array[0] -> GetWindow();
	button_parent[12] = main_window_array[1] -> GetWindow();
	button_parent[13] = main_window_array[2] -> GetWindow();
	button_parent[14] = main_window_array[2] -> GetWindow();

	for (i = 0;i < BUTTON_NUMBER;i++){
		if (i == 13 || i == 14){
			button_x_value = 10;
		}
		if (i == 12){
			button_x_value = 70;	
		}
		if (i == 10){
			button_x_value = 20;
		}
		if (i == 11){
			button_x_value = 90;
		}
		if (i == 7 || i == 8){
			button_x_value = 20;
		}
		if (i == 4 || i == 9){
			button_x_value = 15;
		}
		if (i == 3){
			button_x_value = 10;
		}
		if (i == 5 || i == 6){
			button_x_value = 48;
		}
		button_array[i] = new Button(BUTTON_WIDTH,BUTTON_HEIGHT,
									 button_x_value,button_y_array[i],	
									 button_color,button_parent[i],
									 BUTTON_BORDER_WIDTH,
									 font_name,font_color_name,
									 display,screen_number);
		button_array[i] -> SetText(button_text[i]);
		if (i != 7 || i != 8 || i != 5 || i != 6){
			button_array[i] -> ShowWindow();
		}
	}
	/* Create CheckBoxes */
	for (i = 0;i < CHECK_BOX_NUMBER;i++){
		check_box_parent[i] = check_box_panel_array[i] -> GetWindow();
	}
	for (i = 0;i < CHECK_BOX_NUMBER;i++){
		check_box_array[i] = new CheckBox(CHECK_BOX_WIDTH,CHECK_BOX_HEIGHT,
										  check_box_x_value,check_box_y_value,
										  check_box_color,check_box_parent[i],
										  display,screen_number,
										  CHECK_BOX_BORDER_WIDTH);
		if(i !=4 || i != 5 || i != 6 || 
		   i !=7 || i != 8 || i != 9 || i != 10){
			check_box_array[i] -> ShowWindow();
		}
		/* default time domain is active */
		if(i == 1){
			check_box_array[i] -> Check();
		}
		/* default usual noise is active */
		if(i == 2){
			check_box_array[i] -> Check();
			
		}
		/* default sine frequency */
		if(i == 7){
			check_box_array[i] -> Check();
		}
		/* default recording time is 10 sec */
		if(i == 9){
			check_box_array[i] -> Check();
		}
	}

	/* Create EditBoxes */
	edit_box_parent[0] = edit_box_panel_array[0] -> GetWindow();
	for(i = 0;i < EDIT_BOX_NUMBER;i++){
		edit_box_array[i] = new EditBox(EDIT_BOX_WIDTH,EDIT_BOX_HEIGHT,
										edit_box_x_array[i],edit_box_y_array[i],
										edit_box_color,edit_box_parent[i],
										display,screen_number,
										EDIT_BOX_BORDER_WIDTH,
										font_name,font_color_name);
		edit_box_array[i] -> SetInitialState();
		edit_box_array[i] -> ShowWindow();
	}
	/* Create ProgressBar units */
	progress_bar_parent[0] = main_window_array[2] -> GetWindow();
	for (i = 0;i < PROGRESS_BAR_NUMBER;i++){
		progress_bar_array[i] = new ProgressBar(PROGRESS_BAR_WIDTH,PROGRESS_BAR_HEIGHT,
											    progress_bar_x_array[i],
											    progress_bar_y_array[i],
											    progress_bar_color,progress_bar_parent[i],
											    display,screen_number,
											    PROGRESS_BAR_BORDER_WIDTH,
											    font_name,font_color_name);
		progress_bar_array[i] -> SetReferenceValue(time);
		progress_bar_array[i] -> ShowWindow();
	}
	/* Create TimePanels */
	time_panel_parent[0] = big_panel_array[3] -> GetWindow();
	time_panel_parent[1] = main_window_array[2] -> GetWindow();
	for (i = 0;i < TIME_PANEL_NUMBER;i++){
		time_panel_array[i] = new TimePanel(TIME_PANEL_WIDTH,TIME_PANEL_HEIGHT,
											time_panel_x_array[i],
											time_panel_y_array[i],
											time_panel_color,time_panel_parent[i],
											display,screen_number,
											TIME_PANEL_BORDER_WIDTH,
											font_name,font_color_name);
		time_panel_array[i] -> SetTopValue(time); 
		time_panel_array[i] -> ShowWindow();
	}
	/* Create Canvas units */
	canvas_parent[0] = main_window_array[2] -> GetWindow();
	for (i = 0;i < CANVAS_NUMBER;i++){
		canvas_array[i] = new Canvas(CANVAS_WIDTH,CANVAS_HEIGHT,
									 canvas_x_array[i],
									 canvas_y_array[i],
									 canvas_color,canvas_parent[i],
									 display,screen_number,
									 CANVAS_BORDER_WIDTH,
									 font_name,font_color_name);			
		canvas_array[i] -> SetArrayValues(frames,frame_length);
		canvas_array[i] -> ShowWindow();
	}
	/* Hide some windows */
	/* Sine wave options */
	big_panel_array[5] -> HideWindow();
	button_array[6] -> HideWindow();
	for (i = 4;i <= 7;i++){
		check_box_array[i] -> HideWindow();
		check_box_panel_array[i] -> HideWindow();
	}
	/* Recording */
	big_panel_array[7] -> HideWindow();
	button_array[7] -> HideWindow();
	button_array[8] -> HideWindow();
	/* Time during recording */
	big_panel_array[6] -> HideWindow();
	button_array[6] -> HideWindow();
	for (i = 8;i <= 10;i++){
		check_box_array[i] -> HideWindow();
		check_box_panel_array[i] -> HideWindow();
	}
	/* Creating capture and playback objects */
	apb = new AlsaPlayback();
	acp = new AlsaCapture();
	//apb -> SetTimerValue(time);
	//acp -> SetTimerValue(time);
	//apb -> SetProgressBar(progress_bar_array[0]);
	apb -> SetSineFreq(sine_freq);
	apb -> SetTimePanel(time_panel_array[1]);
	apb -> ShmInit();
	acp -> ShmInit();
	acp -> SetTimePanel(time_panel_array[0]);
	apb -> SetCanvas(canvas_array[0]);
	acp -> SetCanvas(canvas_array[0]);
	acp -> SetCaptureTime(time);
	apb -> SetPlaybackTime(time);
	acp -> ResetTimer();
	apb -> ResetTimer();
	while(!done){
		XNextEvent(main_window_array[0] -> GetDisplay(),&event);
		switch(event.type){
			case Expose:
				for (i = 0;i < MAIN_WINDOW_NUMBER;i++){
					main_window_array[i] -> DrawText(text);
				}
				/*
				for (i = 0;i < CANVAS_NUMBER;i++){
					canvas_array[i] -> PlotArray(data);
				}
				*/
				for (i = 0;i < BUTTON_NUMBER;i++){
					button_array[i] -> ButtonReleasedState();
					button_array[i] -> DrawText();
				
				}
				for (i = 0;i < BIG_PANEL_NUMBER;i++){
					big_panel_text_x = big_panel_array[i] -> GetWidth() * 2/5;
					big_panel_text_y = big_panel_array[i] -> GetHeight()/9;
					if (i == 2){
						big_panel_text_x = big_panel_array[i] -> GetWidth() * 1/5;	
						big_panel_text_y = big_panel_array[i] -> GetHeight()/8;
					}
					if (i == 3){
						big_panel_text_y = big_panel_array[i] -> GetHeight()/7 + 5;
						big_panel_text_x = big_panel_text_x - 20;
					}
					if (i == 0){
						big_panel_text_y = big_panel_array[i] -> GetHeight()/9 + 5;
						big_panel_text_x = big_panel_array[i] -> GetWidth()/5 + 10;
					}
					if (i == 4){
						big_panel_text_y = big_panel_array[i] -> GetHeight()/9 + 8;
						big_panel_text_x = big_panel_array[i] -> GetWidth()/5 + 10;
					}
					big_panel_array[i] -> DrawText(big_panel_text_x,big_panel_text_y);
				}
				for (i = 0;i < CHECK_BOX_PANEL_NUMBER;i++){
					check_box_panel_array[i] -> DrawText(5,check_box_panel_array[i] -> GetHeight()*2/3);	
				}
				for (i = 0;i < EDIT_BOX_PANEL_NUMBER;i++){
					edit_box_panel_array[i] -> DrawText(edit_box_panel_array[i] -> GetWidth() * 1/10,
														edit_box_panel_array[i] -> GetHeight()*1/4);
				}
				for (i = 0;i < CHECK_BOX_NUMBER;i++){
					check_box_array[i] -> Draw();
				}
				for (i = 0;i < EDIT_BOX_NUMBER;i++){
					edit_box_array[i] -> Draw();
				}
				for (i = 0;i < PROGRESS_BAR_NUMBER;i++){
					progress_bar_array[i] -> Draw();
				}
				for (i = 0;i < TIME_PANEL_NUMBER;i++){
					time_panel_array[i] -> Draw();
				}
				break;
			case KeyPress:
				if (event.xkey.keycode == 9){
					done = true;
				}
				for (i = 0;i < EDIT_BOX_NUMBER;i++){
					if ((event.xkey.keycode == 36) && edit_box_array[i] -> GetState()){
						edit_box_array[i] -> Unclick();
						edit_box_array[i] -> Draw();
						sine_freq = atoi(edit_box_array[i] -> GetText());
						fprintf(stderr,"Frequency is: %i\n",sine_freq);
						apb -> SetSineFreq(sine_freq);

					}
					if (event.xkey.keycode != 36 && 
					   event.xkey.keycode != 9 &&  
					   event.xkey.keycode != 22 &&
					   edit_box_array[i] -> GetState())
					   {
						XLookupString(&event.xkey,&ch,1,&key_sym,NULL);
					   	edit_box_array[i] -> PrintChar(ch);
						edit_box_array[i] -> Draw();
					   }
					if (event.xkey.keycode != 36 &&
						event.xkey.keycode != 9 &&
						event.xkey.keycode == 22 &&
						edit_box_array[i] -> GetState()){
							edit_box_array[i] -> DeleteCharLeft();
							edit_box_array[i] -> Draw();
						}
				}
				break;
			case ButtonPress:
				for (i = 0;i < CHECK_BOX_NUMBER;i++){
					if (event.xbutton.window == check_box_array[i] -> GetWindow()){
						if (i == 0 || i == 1){
							if(i == 0){
								check_box_array[0] -> Check();
								check_box_array[1] -> Uncheck();
							 	time_domain = false;
								fprintf(stderr,"FrequencyDomain\n");
							}
							if(i == 1){
								check_box_array[0] -> Uncheck();
								check_box_array[1] -> Check();
								time_domain = true;
								fprintf(stderr,"TimeDomain\n");
							}
						}
						fprintf(stderr,"i = %i\n",i);
						if (i == 2 || i == 3){
							if (i == 2){
								check_box_array[2] -> Check();
								check_box_array[3] -> Uncheck();
								fprintf(stderr,"Usual noise\n");
								apb -> SetNoiseType(USUAL_NOISE);
							}
							if (i == 3){
								check_box_array[2] -> Uncheck();
								check_box_array[3] -> Check();
								fprintf(stderr,"Voice-like noise\n");
								apb -> SetNoiseType(VOICE_LIKE_NOISE);
							}
						}
						if (i == 4 || i == 5 || i == 6 || i == 7){
							if (i == 4){
								check_box_array[4] -> Check();
								check_box_array[5] -> Uncheck();	
								check_box_array[6] -> Uncheck();
								check_box_array[7] -> Uncheck();
								sine_freq = freq_array[i-4];
								apb -> SetSineFreq(sine_freq);
								fprintf(stderr,"Sine freq is: %i\n",sine_freq);
							}
							if (i == 5){
								check_box_array[4] -> Uncheck();
								check_box_array[5] -> Check();
								check_box_array[6] -> Uncheck();
								check_box_array[7] -> Uncheck();
								sine_freq = freq_array[i-4];
								apb -> SetSineFreq(sine_freq);
								fprintf(stderr,"Sine freq is: %i\n",sine_freq);
							}
							if (i == 6){
								check_box_array[4] -> Uncheck();
								check_box_array[5] -> Uncheck();
								check_box_array[6] -> Check();
								check_box_array[7] -> Uncheck();
								sine_freq = freq_array[i-4];
								apb -> SetSineFreq(sine_freq);
								fprintf(stderr,"Sine freq is: %i\n",sine_freq);
							}
							if (i == 7){
								check_box_array[4] -> Uncheck();
								check_box_array[5] -> Uncheck();
								check_box_array[6] -> Uncheck();
								check_box_array[7] -> Check();
								sine_freq = freq_array[i-4];
								apb -> SetSineFreq(sine_freq);
								fprintf(stderr,"Sine freq is: %i\n",sine_freq);
								
							}
						}
						if (i == 8 || i == 9 || i == 10){
							if (i == 8){
								check_box_array[8] -> Check();
								check_box_array[9] -> Uncheck();
								check_box_array[10] -> Uncheck();
								time = time_array[i-8];
								fprintf(stderr,"Time is: %i\n",time);
								for (j = 0;j < TIME_PANEL_NUMBER;j++){ 
									time_panel_array[j] -> SetTopValue(time);
									time_panel_array[j] -> Draw();
								}
								progress_bar_array[0] -> SetReferenceValue(time);
								progress_bar_array[0] -> Draw();
								acp -> SetCaptureTime(time);
								apb -> SetPlaybackTime(time);
							}
							if (i == 9){
								check_box_array[8] -> Uncheck();
								check_box_array[9] -> Check();
								check_box_array[10] -> Uncheck();
								time = time_array[i-8];
								fprintf(stderr,"Time is: %i\n",time);
								for (j = 0;j < TIME_PANEL_NUMBER;j++){ 
									time_panel_array[j] -> SetTopValue(time);
									time_panel_array[j] -> Draw();
								}
								progress_bar_array[0] -> SetReferenceValue(time);
								progress_bar_array[0] -> Draw();
								acp -> SetCaptureTime(time);
								apb -> SetPlaybackTime(time);
							}
							if (i == 10){
								check_box_array[8] -> Uncheck();
								check_box_array[9] -> Uncheck();
								check_box_array[10] -> Check();
								time = time_array[i-8];
								fprintf(stderr,"Time is: %i\n",time);
								for (j = 0;j < TIME_PANEL_NUMBER;j++){ 
									time_panel_array[j] -> SetTopValue(time);
									time_panel_array[j] -> Draw();
								}
								progress_bar_array[0] -> SetReferenceValue(time);
								progress_bar_array[0] -> Draw();
								acp -> SetCaptureTime(time);
								apb -> SetPlaybackTime(time);
							}
						
						}
					}
				}
				for (i = 0;i < BUTTON_NUMBER;i++){
					if (event.xbutton.window == button_array[i] -> GetWindow())
						button_array[i] -> ButtonPressedState();
				}
				for (i = 0;i < EDIT_BOX_NUMBER;i++){
					if (event.xbutton.window == edit_box_array[i] -> GetWindow()){
						edit_box_array[i] -> Click();
						edit_box_array[i] -> Draw();
					}
				}
				if (event.xbutton.window == button_array[0] -> GetWindow()){
					//progress_bar_array[0] -> CheckForIncrease(counter);
					//progress_bar_array[0] -> Draw();
					//time_panel_array[0] -> RenewCounter(counter);
					//time_panel_array[0] -> Draw();
					//fprintf(stderr,"Counter = %i\n",counter);
					//counter++;
					main_window_array[2] -> ShowWindow();
					time_panel_array[1] -> Start();
					apb -> PlayNoise();
				}
				if (event.xbutton.window == button_array[8] -> GetWindow()){
					apb -> PlayVoice();
				}
				if (event.xbutton.window == button_array[1] -> GetWindow()){
					//progress_bar_array[0] -> ResetBar();
					//progress_bar_array[0] -> Draw();
					//apb -> ResetTimer();
					//counter = 1;
					apb -> PlaySine();
				}
				if (event.xbutton.window == button_array[9] -> GetWindow()){
					
					acp -> ResetTimer();
					time_panel_array[0] -> Stop();
					time_panel_array[0] -> SetTopValue(time);
					time_panel_array[0] -> Draw();
					//counter = 1;
				}
				if (event.xbutton.window == button_array[4] -> GetWindow()){
					big_panel_array[7] -> ShowWindow();
					button_array[7] -> ShowWindow();
					button_array[8] -> ShowWindow();
					button_array[7] -> ButtonReleasedState();
					button_array[8] -> ButtonReleasedState();
					button_array[7] -> DrawText();
					button_array[8] -> DrawText();
					time_panel_array[0] -> Start();
					acp -> CaptureVoice();
				}
				if (event.xbutton.window == button_array[7] -> GetWindow()){
					acp -> ResetTimer();
					time_panel_array[0] -> Stop();
					time_panel_array[0] -> SetTopValue(time);
					time_panel_array[0] -> Draw();
					big_panel_array[7] -> HideWindow();
					button_array[7] -> HideWindow();
					button_array[8] -> HideWindow();
				}
				if (event.xbutton.window == button_array[2] -> GetWindow()){
					big_panel_array[5] -> ShowWindow();
					button_array[5] -> ShowWindow();
					for (i = 4;i <= 7;i++){
						check_box_array[i] -> ShowWindow();
						check_box_panel_array[i] -> ShowWindow();
					}
				}
				if (event.xbutton.window == button_array[5] -> GetWindow()){
					big_panel_array[5] -> HideWindow();
					button_array[5] -> HideWindow();
					for (i = 4;i <= 7;i++){
						check_box_array[i] -> HideWindow();
						check_box_panel_array[i] -> HideWindow();
					}
				}
				if (event.xbutton.window == button_array[3] -> GetWindow()){
					big_panel_array[6] -> ShowWindow();
					button_array[6] -> ShowWindow();
					for (i = 8;i <= 10;i++){
						check_box_array[i] -> ShowWindow();
						check_box_panel_array[i] -> ShowWindow();
					}
				}
				if (event.xbutton.window == button_array[6] -> GetWindow()){
					big_panel_array[6] -> HideWindow();
					button_array[6] -> HideWindow();
					for (i = 8;i <= 10;i++){
						check_box_array[i] -> HideWindow();
						check_box_panel_array[i] -> HideWindow();
					}
				}
				if (event.xbutton.window == button_array[11] -> GetWindow()){
					main_window_array[1] -> ShowWindow();
				}
				if (event.xbutton.window == button_array[12] -> GetWindow()){
					main_window_array[1] -> HideWindow();
				}
				if (event.xbutton.window == button_array[10] -> GetWindow()){
					done = true;
				}
				if (event.xbutton.window == button_array[13] -> GetWindow()){
					main_window_array[2] -> HideWindow();
				}
				if (event.xbutton.window == button_array[14] -> GetWindow()){
					
					apb -> ResetTimer();
					time_panel_array[1] -> Stop();
					time_panel_array[1] -> SetTopValue(time);
					time_panel_array[1] -> Draw();
				
				}
				break;
			case ButtonRelease:
				for (i = 0;i < BUTTON_NUMBER;i++){
					if(event.xbutton.window == button_array[i] -> GetWindow())
						button_array[i] -> ButtonReleasedState();
				}
				break;
			default:
				break;
		}
	}
	for (i = 0;i < MAIN_WINDOW_NUMBER;i++){
		delete main_window_array[i];
	}
	for (i = 0;i < CHECK_BOX_NUMBER;i++){
		delete check_box_array[i];	
	}
	for (i = 0;i < BUTTON_NUMBER;i++){
		delete button_array[i];	
	}
	for (i = 0;i < CHECK_BOX_PANEL_NUMBER;i++){
		delete check_box_panel_array[i];
	}
	for (i = 0;i < BIG_PANEL_NUMBER;i++){
		delete big_panel_array[i];
	}
	for (i = 0;i < EDIT_BOX_PANEL_NUMBER;i++){
		delete edit_box_panel_array[i];
	}
	for (i = 0;i < EDIT_BOX_NUMBER;i++){
		delete edit_box_array[i];
	}
	for (i = 0;i < PROGRESS_BAR_NUMBER;i++){
		delete progress_bar_array[i];
	}
	for (i = 0;i < TIME_PANEL_NUMBER;i++){
		delete time_panel_array[i];
	}
	for (i = 0;i < CANVAS_NUMBER;i++){
		delete canvas_array[i];
	}
	delete acp;
	delete apb;
	XCloseDisplay(display);
	return 0;
}
