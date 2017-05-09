/* x11_classes.cpp
 *
 * Implementation of widgets in x11
 *
 */

#include "x11_classes.h"

/* Methods for AbstractWindow class */
void AbstractWindow::SetTextProperty(char const *font_name,char const *font_color_name)
{
	wind_text_font = XLoadFont(display,(char *)font_name);
	wind_text_item.font = wind_text_font;
	wind_text_item.delta = 0;
	XParseColor(display,wind_colormap,(char *)font_color_name,&wind_color);
	XAllocColor(display,wind_colormap,&wind_color);
	valuemask = GCForeground;
	wind_gc_values.foreground = wind_color.pixel;
	wind_gc = XCreateGC(display,cur_window,valuemask,&wind_gc_values);
}

void AbstractWindow::SetWindowColor(char const *color_name)
{
	XParseColor(display,wind_colormap,(char *)color_name,&wind_color);
	XAllocColor(display,wind_colormap,&wind_color);
	valuemask = GCForeground | GCLineWidth;
	wind_gc_values.line_width = 2;
	wind_gc_values.foreground = wind_color.pixel;
	wind_gc = XCreateGC(display,cur_window,valuemask,&wind_gc_values);
}


void AbstractWindow::CreateWindow(unsigned int width_p,unsigned int height_p,
								 int x_p,int y_p,char const *color_str_p,
								 Window parent_p,unsigned int border_width_p,
								 Display *display_p,int screen_number_p)
{
	display = display_p;
	screen_number = screen_number_p;
	width = width_p;
	height = height_p;
	border_width = border_width_p;
	x = x_p;
	y = y_p;
	wind_class = InputOutput;
	depth = DefaultDepth(display,screen_number);
	visual = DefaultVisual(display,screen_number);
	parent = parent_p;
	/* Set hints for window manager */
	size_hints.flags = USPosition | USSize;
	wm_hints.initial_state = NormalState;
	wm_hints.flags = StateHint;
	/* Set color of the window */
	valuemask = CWBackPixel | CWBorderPixel | CWEventMask;
	color_str = (char *)color_str_p;
	wind_colormap = DefaultColormap(display,screen_number);
	XParseColor(display,wind_colormap,(char *)color_str,&wind_color);
	XAllocColor(display,wind_colormap,&wind_color);
	wind_attr.background_pixel = wind_color.pixel;
	wind_attr.border_pixel = BlackPixel(display,screen_number);
	wind_attr.event_mask = event_mask;
	cur_window = XCreateWindow(display,parent,x,y,width,height,
							   border_width,depth,wind_class,
							   visual,valuemask,&wind_attr);
	XSetWMNormalHints(display,cur_window,&size_hints);
	XSetWMHints(display,cur_window,&wm_hints);
	HideWindow();
}

void AbstractWindow::ShowWindow()
{
	XMapWindow(display,cur_window);
}

void AbstractWindow::HideWindow()
{
	XUnmapWindow(display,cur_window);
}


/* Methods for MainWindow class */


MainWindow::MainWindow(unsigned int width_p,unsigned int height_p,
					   int x_p,int y_p,char const *color_str_p,
					   char const *title_p,char const *font_name,
					   char const *font_color_name,Display *display_p,
					   int screen_number_p)
{
	event_mask = KeyPressMask | ExposureMask;
	CreateWindow(width_p,height_p,x_p,y_p,color_str_p,
				 RootWindow(display_p,screen_number_p),2,
				 display_p,screen_number);
	/* Set title of the window */
	title = (char *)title_p;
	XStringListToTextProperty(&title,1,&wind_name);
	XSetWMName(display,cur_window,&wind_name);
	SetTextProperty(font_name,font_color_name);
}


void MainWindow::DrawText(char const *text)
{
	wind_text_item.chars = (char *)text;
	wind_text_item.nchars = strlen((char *)text);
	XDrawText(display,cur_window,wind_gc,30,30,&wind_text_item,1);
}


/* Button class methods */

Button::Button(unsigned int width_p,unsigned int height_p,int x_p,int y_p,
			   char const *color_str_p,Window parent_p,
			   unsigned int border_width_p,const char *font_name,
			   const char *font_color_name,Display *display_p,
			   int screen_number_p)
{
	event_mask = KeyPressMask | ExposureMask | ButtonPressMask | ButtonReleaseMask;
	CreateWindow(width_p,height_p,x_p,y_p,color_str_p,parent_p,
	             border_width,display_p,screen_number_p);
	SetWindowColor("white smoke");
	button_released_gc = wind_gc;
	SetWindowColor("black");
	button_pressed_gc = wind_gc;
	SetTextProperty(font_name,font_color_name);
	ButtonReleasedState();
}

void Button::SetText(const char *btn_text)
{
	button_text = (char *)btn_text;
}

void Button::DrawText()
{

	wind_text_item.chars = button_text;
	wind_text_item.nchars = strlen(button_text);
	XDrawText(display,cur_window,wind_gc,width/5,height*3/5,&wind_text_item,1);
}

void Button::ButtonPressedState()
{
	/* use button_pressed_gc for left 
	   and top side of button */
	XDrawLine(display,cur_window,
			  button_pressed_gc,0,0,
			  0,height);
	XDrawLine(display,cur_window,
			  button_pressed_gc,0,0,
			  width,0);
	/* use button_relesed_gc for right
	   and bottom side of button */
	XDrawLine(display,cur_window,
			  button_released_gc,width,0,
			  width,height);
	XDrawLine(display,cur_window,
			  button_released_gc,0,height,
			  width,height);
}

void Button::ButtonReleasedState()
{
	/* use button_released_gc for left 
	   and top side of button */
	XDrawLine(display,cur_window,
			  button_released_gc,0,0,
			  0,height);
	XDrawLine(display,cur_window,
			  button_released_gc,0,0,
			  width,0);
	/* use button_pressed_gc for right
	   and bottom side of button */
	XDrawLine(display,cur_window,
			  button_pressed_gc,width,0,
			  width,height);
	XDrawLine(display,cur_window,
			  button_pressed_gc,0,height,
			  width,height);
}

/* methods of Panel class */

Panel::Panel(unsigned int width_p,unsigned int height_p,int x_p,int y_p,
		      char const *color_str_p,char const *font_name,
			  char const *font_color_name,Window parent_p,
			  Display *display_p,int screen_number_p,unsigned int border_width_p)
{
	event_mask = 0;
	CreateWindow(width_p,height_p,x_p,y_p,color_str_p,parent_p,
	             border_width_p,display_p,screen_number_p);
	SetTextProperty(font_name,font_color_name);

}

void Panel::SetText(char const *capt_text)
{
	caption_text = (char *)capt_text;
}

void Panel::DrawText(int x_p,int y_p)
{
	wind_text_item.chars = caption_text;
	wind_text_item.nchars = strlen(caption_text);
	XDrawText(display,cur_window,wind_gc,x_p,y_p,&wind_text_item,1);
}

/* methods of CheckBox class */


CheckBox::CheckBox(unsigned int width_p,unsigned int height_p,
				   int x_p,int y_p, char const *color_str_p,
				   Window parent_p, Display *display_p,int screen_number_p,
			       unsigned int border_width_p)
{
	event_mask = ExposureMask | ButtonPressMask;
	CreateWindow(width_p,height_p,x_p,y_p,color_str_p,
	             parent_p,border_width_p,display_p,screen_number_p);	
	SetWindowColor("black");
	is_checked = false;
}

void CheckBox::ChangeCheckState()
{
	if (is_checked){
		Uncheck();
	} else {
		Check();
	}
}

void CheckBox::Check()
{
	XDrawLine(display,cur_window,wind_gc,
			  0,0,width,height);
	XDrawLine(display,cur_window,wind_gc,
			  width,0,0,height);
	is_checked = true;
}

void CheckBox::Uncheck()
{
	XClearWindow(display,cur_window);
	is_checked = false;
}

void CheckBox::Draw()
{
	if (is_checked){
		Check();
	} else {
		Uncheck();
	}
}

/* methods of EditBox class */

EditBox::EditBox(unsigned int width_p,unsigned int height_p,
				 int x_p,int y_p,char const *color_str_p,
				 Window parent_p,Display *display_p,int screen_number_p,
				 unsigned int border_width_p,const char *font_name_p,
				 const char *font_color_name_p)
{
	event_mask = ExposureMask | KeyPressMask | ButtonPressMask;
	CreateWindow(width_p,height_p,x_p,y_p,color_str_p,
				 parent_p,border_width_p,display_p,
				 screen_number_p);
	SetTextProperty(font_name_p,font_color_name_p);
}


void EditBox::SetInitialState()
{
	int i;
	is_active = false;
	pix_per_char = 10;
	max_char_numb = (int) (width / pix_per_char);
	text = new char[max_char_numb];
	cur_pos = 0;
	for(i = 0;i < max_char_numb;i++)
		text[i] = ' ';
}

void EditBox::PrintChar(char ch)
{
	if(cur_pos + 1 < max_char_numb){
		cur_pos++;
		text[cur_pos] = ch;
	}
}

void EditBox::DeleteCharLeft()
{
	text[cur_pos] = ' ';
	cur_pos--;
}

void EditBox::Draw()
{
	wind_text_item.chars = text;
	wind_text_item.nchars = strlen(text);
	XClearWindow(display,cur_window);
	XDrawText(display,cur_window,wind_gc,
			  0,height*4/5,&wind_text_item,1);
	if(is_active){
		XDrawLine(display,cur_window,wind_gc,
				  pix_per_char * cur_pos + (pix_per_char)/2 + 1,height * 1/5 + 1,
				  pix_per_char * cur_pos + (pix_per_char)/2 + 1,height * 4/5 + 1);
	}
}

void EditBox::Click()
{
	is_active = true;
	SetWindowColor("light gray");
	XSetWindowBackground(display,cur_window,wind_color.pixel);
	SetWindowColor("black");
}

void EditBox::Unclick()
{
	is_active = false;
	SetWindowColor("white");
	XSetWindowBackground(display,cur_window,wind_color.pixel);
	SetWindowColor("black");
}

/* methods of ProgressBar class */

ProgressBar::ProgressBar(unsigned int width_p,unsigned int height_p,
						 int x_p,int y_p,char const *color_str_p,
			 			 Window parent_p,Display *display_p,int screen_number_p,
						 unsigned int border_width_p,const char *font_name_p,
			 			 const char *font_color_p)
{
	event_mask = ExposureMask;
	CreateWindow(width_p,height_p,x_p,y_p,color_str_p,
				 parent_p,border_width_p,display_p,
				 screen_number_p);
	SetTextProperty(font_name_p,font_color_p);
	text = (char *)"Progress";
	reset_color = color_str_p;
	cur_progress = 0;
}

void ProgressBar::SetReferenceValue(int val)
{
	input_value = val;
	step = (int)((width * 19/20) / input_value); 
	
}

void ProgressBar::CheckForIncrease(int control_value)
{
	if ((control_value * step >= cur_progress)&& 
		 (cur_progress < step * input_value)){
		StepUp();
	}
}

void ProgressBar::StepUp()
{
	cur_progress += step;
}

void ProgressBar::Draw()
{
	wind_text_item.chars = (char *)text;
	wind_text_item.nchars = strlen(text);
	XDrawText(display,cur_window,wind_gc,
			  width/3,height/4,&wind_text_item,1);	
	SetWindowColor("green");/* default color for progress bar */
	XFillRectangle(display,cur_window,wind_gc,
				   width/20,height*3/5,cur_progress,
				   20);/* draw progress bar */
	SetWindowColor("black");/* default color for font */
	XDrawRectangle(display,cur_window,wind_gc,
				   width/20,height*3/5,step * input_value,
				   20);/* draw progress bar */

}

void ProgressBar::ResetBar()
{
	SetWindowColor(reset_color);
	XFillRectangle(display,cur_window,wind_gc,
				   width/20,height*3/5,cur_progress,
				   20);/* draw progress bar */
	SetWindowColor("black");/* default color for font */
	cur_progress = 0;

}

/* methods for TimePanel class */

TimePanel::TimePanel(unsigned int width_p,unsigned int height_p,
				     int x_p,int y_p,char const *color_str_p,
				     Window parent_p,Display *display_p,int screen_number_p,
				     unsigned int border_width_p,const char *font_name_p,
				     const char *font_color_name_p)
{
	event_mask = ExposureMask;
	CreateWindow(width_p,height_p,x_p,y_p,color_str_p,
				 parent_p,border_width_p,display_p,
				 screen_number_p);
	SetTextProperty(font_name_p,font_color_name_p);

	reset_color = color_str_p;
	active_color = "green";
	passive_color = "red";
	caption = "Status";
	time_string = new char[5];
	is_active = false;
	Reset();
}


void TimePanel::SetTopValue(int top_v)
{
	top_value = top_v;
	sprintf(&time_string[3],"%d",top_value/10);
	sprintf(&time_string[4],"%d",top_value%10);
	time_string[0] = '-';
	time_string[1] = '-';
	time_string[2] = '/';
}

void TimePanel::Reset()
{
	time_string[0] = '-';
	time_string[1] = '-';
	time_string[2] = '/';
	time_string[3] = '-';
	time_string[4] = '-';
}
void TimePanel::Start()
{
	is_active = true;
}

void TimePanel::Stop()
{
	is_active = false;
	Reset();
	
}

void TimePanel::RenewCounter(int val)
{
	if (val <= top_value){
		sprintf(&time_string[0],"%d",val/10);
		sprintf(&time_string[1],"%d",val%10);
		time_string[2] = '/';
		sprintf(&time_string[3],"%d",top_value/10);
		sprintf(&time_string[4],"%d",top_value%10);
	}
}

void TimePanel::Draw()
{
	/* clear area for new digits */
	SetWindowColor(reset_color);
	XFillRectangle(display,cur_window,wind_gc,
				   width/7,height * 3/5,40,height/5);
	SetWindowColor("black");/* default font color */
	wind_text_item.chars = (char *)caption;
	wind_text_item.nchars = strlen((char *)caption);
	XDrawText(display,cur_window,wind_gc,
			  width/5,height/3,&wind_text_item,1);
	wind_text_item.chars = (char *)time_string;
	wind_text_item.nchars = strlen((char *)time_string);
	XDrawText(display,cur_window,wind_gc,
			  width/7,height * 4/5,&wind_text_item,1);
	if (is_active){
		SetWindowColor(active_color);
		XFillArc(display,cur_window,wind_gc,
				 width*5/7,height*3/5,13,13,
				 0,360*64);
	} else {
		SetWindowColor(passive_color);
		XFillArc(display,cur_window,wind_gc,
				 width*5/7,height*3/5,13,13,
				 0,360*64);
	}
	SetWindowColor("black");/* default font color */

}

/* methods of canvas class */

Canvas::Canvas(unsigned int width_p,unsigned int height_p,
			   int x_p,int y_p,char const *color_str_p,
			   Window parent_p,Display *display_p,int screen_number_p,
			   unsigned int border_width_p,const char *font_name_p,
			   const char *font_color_p)
{
	event_mask = 0;
	CreateWindow(width_p,height_p,x_p,y_p,color_str_p,
				 parent_p,border_width_p,display_p,
				 screen_number_p);
	SetWindowColor("navy blue");

}

Canvas::~Canvas()
{
	delete data;
	delete array_copy;
}

void Canvas::CalculateCoordUnits()
{
	y_unit = (float)height / (float)max_y_value;	
	x_unit = (float) width / (float)(data_size - 1);
	fprintf(stderr,"x_unit: %f\n",x_unit);
	fprintf(stderr,"y_unit: %f\n",y_unit);
}

void Canvas::DrawPoint(int x,int y)
{
	XDrawPoint(display,cur_window,wind_gc,x,y);		
}

void Canvas::DrawPoints()
{
	int i;
	int x_value;
	int x_prev_value;
	for (i = 0;i < data_size;i++){
		x_value = (int)(x_unit * (float)(i));
		DrawPoint(x_value,data[i]);
		if (i >= 1  && i < data_size){
			x_prev_value = (int)(x_unit * (float)(i-1));
			XDrawLine(display,cur_window,wind_gc,
					  x_prev_value,data[i-1],
					  x_value,data[i]);	
		}
	}
}

void Canvas::SetArrayValues(int array_size,int max_y)
{
	SetMaxYValueMeasure(max_y * 2);
	data_size = array_size;
	data = new int[data_size];
	array_copy = new short int[data_size];
	CalculateCoordUnits();
}

void Canvas::PlotArray(short int *array)
{
	int i;
	/* convert values to canvas coordinates 
	   with notice that y in XWindowSystem
	   is measured relative to up - left corner
	*/
	int tmp;
	for (i = 0;i < data_size;i++){
		if (array[i] < 0){
			array_copy[i] = array[i] + max_y_value / 2;/* shift graph up */
			data[i] = height - (int)((float)array_copy[i] * (float)y_unit);					
		} else {
			array_copy[i] = array[i];
			data[i] = height / 2 - (int)((float)array_copy[i] * (float)y_unit);					
		}
	}
	DrawPoints();
}

void Canvas::ClearCanvas()
{
	XClearWindow(display,cur_window);
}


