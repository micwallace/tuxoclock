#include <pebble.h>
	
static Window *s_main_window;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static TextLayer *s_hours_layer;
static TextLayer *s_minutes_layer;
static TextLayer *s_date_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char hours[] = "00";
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(hours, sizeof("00"), "%H", tick_time);
  } else {
    // Use 12 hour format
    strftime(hours, sizeof("00"), "%I", tick_time);
  }
	static char minutes[] = "00";
	static char date[] = "00/00";
	strftime(minutes, sizeof("00"), "%M", tick_time);
	strftime(date, sizeof("00/00"), "%d/%m", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_hours_layer, hours);
	text_layer_set_text(s_minutes_layer, minutes);
	text_layer_set_text(s_date_layer, date);
}

static void main_window_load(Window *window) {
	// Create GBitmap, then set to created BitmapLayer
	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TUX_WATCH);
	s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
	// Create time TextLayers
  s_hours_layer = text_layer_create(GRect(2, 2, 70, 36));
	s_minutes_layer = text_layer_create(GRect(72, 2, 70, 36));
	s_date_layer = text_layer_create(GRect(0, 85, 144, 40));
  text_layer_set_background_color(s_hours_layer, GColorClear);
	text_layer_set_background_color(s_minutes_layer, GColorClear);
	text_layer_set_background_color(s_date_layer, GColorClear);
	#ifdef PBL_COLOR
  	text_layer_set_text_color(s_hours_layer, GColorRed);
		text_layer_set_text_color(s_minutes_layer, GColorRed);
		text_layer_set_text_color(s_date_layer, GColorBlue);
	#else
  	text_layer_set_text_color(s_hours_layer, GColorBlack);
		text_layer_set_text_color(s_minutes_layer, GColorBlack);
		text_layer_set_text_color(s_date_layer, GColorBlack);
	#endif
  // Make sure the time is displayed from the start
	update_time();
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_hours_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
	text_layer_set_font(s_minutes_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
	text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(s_hours_layer, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_minutes_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hours_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minutes_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
	// Destroy TextLayer
  text_layer_destroy(s_hours_layer);
	text_layer_destroy(s_minutes_layer);
	text_layer_destroy(s_date_layer);
	// Destroy GBitmap
	gbitmap_destroy(s_background_bitmap);
	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

static void init() {
	// Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
	
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
	// Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}