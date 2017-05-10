/* x11_classes.h
 *
 * Implementation of widgets in x11
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

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
		void DrawText(char const *text);
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
	protected:
		void SetMaxYValueMeasure(int val){ max_y_value = val; }
		void CalculateCoordUnits();/* find x_unit and y_unit */
		void DrawPoints();/* plotting of data array */
		void DrawPoint(int x,int y);/* draw point coordinates relative to canvas */
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
		void SetDataColor(char const *col){ data_color = col; }
		void ClearCanvas();
};
