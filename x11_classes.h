/* x11_classes.h
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <fftw.h>
#include <rfftw.h>

#define NUMB_OF_FREQ_BANDS	15
#define DATA_RATE			44100

class AbstractWindow{
	protected:
		Display *display;/* specifies connection to X server */
		unsigned int width;/* width of the window */
		unsigned int height;/* height of the window */
		int x;/* x - position relative to parent */
		int y;/* y - position relative to parent */
		Window parent;/* id of the parent window */
		Window cur_window;/* id of the current window */
		int screen_number;/* screen number for window */
		Visual *visual;/* structure for possible color mapping */
		unsigned int border_width;/* width of border of the current window */
		unsigned long valuemask;/* mask for values */
		int depth;/* depth of the window */
		XSetWindowAttributes wind_attr;/* attributes of the current window */
		long event_mask;/* event mask for window */
		unsigned int wind_class;/* class of the current window */
		XColor wind_color;/* color structure */
		Colormap wind_colormap;/* colormap for the current window */ 
		XTextItem wind_text_item;/* structure for text */
		Font wind_text_font;/* font of the text in window */
		GC wind_gc;/* graphic context of the current window */
		XGCValues wind_gc_values;/* attributes for graphic context */
		char *color_str;/* string for color */
		/* for control of window position */
		XWMHints wm_hints;
		XSizeHints size_hints;
	public:
		/* methods */
		void SetTextProperty(char const *font_name, char const *font_color_name);/* set font and color of the text */
		void SetWindowColor(char const *color_name);/* set color by name */
		void CreateDisplay();/* create display and screen number */
		void CreateWindow(unsigned int width_p,unsigned int height_p,
						  int x_p,int y_p,char const *color_str_p,
						  Window parent_p,unsigned int border_width_p,
						  Display *display_p,int screen_number_p);/* create window */
		Window GetWindow(){ return cur_window; }
		Display *GetDisplay(){ return display; }
		unsigned int GetWidth(){ return width; }
		unsigned int GetHeight(){ return height; }
		void ShowWindow();/* make window visible */
		void HideWindow();/* make window unvisible */
		
};


class MainWindow : public AbstractWindow{
	private:
		XTextProperty wind_name;/* structure for name of current window */
		char *title;/* text name of window */
		char *text;/* some text inside */
	public:
		MainWindow(unsigned int width_p,unsigned int height_p,int x_p,int y_p,
		           char const *color_str_p,char const *title,
				   char const *font_name,char const *font_color_name,
				   Display *display_p,int screen_number_p);
		Display* GetDisplay(){ return display; }
		void DrawText(char const *text,int str_order_numb);
};

class Button : public AbstractWindow{
		private:
			GC button_pressed_gc;
			GC button_released_gc;
			char *button_text;
		public:
			Button(unsigned int width_p,unsigned int heght_p,int x_p,int y_p,
				   char const *color_str_p,Window parent_p,
				   unsigned int border_width_p,char const *font_name,
				   char const *font_color_name,Display *display_p,
				   int screen_number_p);
			void SetText(char const *btn_text);/* set button text */
			void DrawText();
			void ButtonPressedState();
			void ButtonReleasedState();
};

class Panel : public AbstractWindow{
	private:
		char *caption_text;
	public:
		Panel(unsigned int width_p,unsigned int height_p,int x_p,int y_p,
		      char const *color_str_p,char const *font_name,
			  char const *font_color_name,Window parent_p,
			  Display *display_p,int screen_number_p,
			  unsigned int border_width_p);
		void SetText(char const *capt_text);/* set panel caption text */
		void DrawText(int x_p,int y_p);
		void DrawImage(const char *name);/* draw bitmap file */
};

class CheckBox : public AbstractWindow{
	private:
		bool is_checked;
	public:
		CheckBox(unsigned int width_p,unsigned int height_p,
				 int x_p,int y_p, char const *color_str_p,
				 Window parent_p, Display *display_p,int screen_number_p,
			     unsigned int border_width_p);
		bool IsChecked(){ return is_checked; };
		void Check();
		void Uncheck();
		void Draw();/* draw current state */
		void ChangeCheckState();/* check or uncheck */
};

class EditBox : public AbstractWindow{
	private:
		char *text;/* for edit box characters */
		int max_char_numb;/* number of characters in edit box */
		int cur_pos;/* current position of cursor */
		int pix_per_char;/* number of pixels per one character */
		bool is_active;/* if edit box is clicked */
	public:
		EditBox(unsigned int width_p,unsigned int height_p,
				int x_p,int y_p,char const *color_str_p,
				Window parent_p,Display *display_p,int screen_number_p,
				unsigned int border_width_p,const char *font_name_p,
				const char *font_color_name_p);
		~EditBox(){ delete text; }
		char* GetText(){return text;}/* entered text */
		void PrintChar(char ch);/* print char in position x */
		void DeleteCharLeft();/* delete char on the left from cursor */
		void SetInitialState();/* set empty text */
		void Draw();/* Draw chars */
		void Click();/* EditBox is activated */
		void Unclick();/* EditBox is disactivated */
		bool GetState(){ return is_active; }
};

class ProgressBar : public AbstractWindow{
	private:
		int step;/* length of one unit */
		int input_value;/* reference unit */
		const char *text;/* text of progress bar */
		const char *reset_color;/* color of empty progress bar */
		int cur_progress;/* current progress */
	public:
		ProgressBar(unsigned int width_p,unsigned int height_p,
					int x_p,int y_p,char const *color_str_p,
					Window parent_p,Display *display_p,int screen_number_p,
					unsigned int border_width_p,const char *font_name_p,
					const char *font_color_p);
		void SetReferenceValue(int val);
		void StepUp();/* increase on one unit */
		void CheckForIncrease(int control_value);/* do we need to increase on one unit? */
		void ResetBar();/* progress reset */
		void Draw();/* draw progress bar */
};

class TimePanel : public AbstractWindow{
	private:
		int top_value;/* top value of time, seconds */
		bool is_active;/* active state demonstration */
		char *time_string;/* string to be drawn. Format: ss/ss */
		char const *active_color;/* indicates time counting */ 
		char const *passive_color;/* indicates standby */
		char const *reset_color;/* color of background */
		char const *caption;/* caption of panel */
	public:
		TimePanel(unsigned int width_p,unsigned int height_p,
				  int x_p,int y_p,char const *color_str_p,
				  Window parent_p,Display *display_p,int screen_number_p,
				  unsigned int border_width_p,const char *font_name_p,
				  const char *font_color_p);
		~TimePanel(){ delete time_string; }
		void SetTopValue(int top_v);/* set top value for counter*/
		void Start();/* start time counting */
		void RenewCounter(int val);/* set new bottom value */
		void Stop();/* stop time counting */
		void Draw();/* draw the panel */
		void Reset();/* reset the timer */
};

class Canvas : public AbstractWindow{
	private:
		int *data;/* data to be plotting */
		short int *array_copy;/* copy of original array */
		int data_size;/* number of elements in array */
		char const *data_color;/* color of data points */
		int max_y_value;/* maximum y value from measure */
		float x_unit;/* one x - unit relative to canvas */
		float y_unit;/* one y - unit relative to canvas */
		/* for frequency spectrum */
		struct fband_data{
			float fband_magn;
		};
		struct fband_data ddata[NUMB_OF_FREQ_BANDS];
		float freq_bands[NUMB_OF_FREQ_BANDS] = 
		{
			50.00f, 100.00f, 155.56f, 220.0f, 311.13f,
			440.00f, 622.25f, 880.00f, 1244.51f, 1760.00f,
			2489.02f, 3519.95f, 4978.04f, 9956.08f, 19912.16f
		};
	protected:
		void SetMaxYValueMeasure(int val){ max_y_value = val; }
		void CalculateCoordUnits();/* find x_unit and y_unit */
		void DrawPoints();/* plotting of data array */
		void DrawPoint(int x,int y);/* draw point coordinates relative to canvas */
		/* for FFT */
		float FindTallestPeak(int offset,int count,fftw_real *cplx);/* find tallest peak in band */
		void FindFreqBandMagn(short int *array);/* find magnitude of frequency fand data */
		void PlotSpectrum();/* plot frequency spectrum of data */
	public:
		Canvas(unsigned int width_p,unsigned int height_p,
			   int x_p,int y_p,char const *color_str_p,
			   Window parent_p,Display *display_p,int screen_number_p,
			   unsigned int border_width_p,const char *font_name_p,
			   const char *font_color_p);
		~Canvas();
		void SetArrayValues(int array_size,int max_y);/* set data buffer size 
														 and top bound of data 
													   */
		void PlotArray(short int *array);/* plot array with measure y-values */
		void PlotArrayFFT(short int *array);/* plot frequency spectrum of array */
		void SetDataColor(char const *col){ data_color = col; }
		void ClearCanvas();
};
