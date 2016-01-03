#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_pinas_layer;
static GFont s_time_font, s_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Copy date into buffer from tm structure
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  // Show the date
  text_layer_set_text(s_date_layer, date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  
  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 65));
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CRACKED_40));
  
  //Create date font
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorYellow);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  //text_layer_set_text(s_time_layer, "00:00");
  //text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  // Apply new font to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
 
   // Create Pilipinas TextLayer
  s_pinas_layer = text_layer_create(
      GRect(0, 30, bounds.size.w, 30));
 
  text_layer_set_text_color(s_pinas_layer, GColorWhite);
  text_layer_set_background_color(s_pinas_layer, GColorBlue);
  text_layer_set_text(s_pinas_layer, "Pilipinas");
  text_layer_set_font(s_pinas_layer, s_date_font);
  text_layer_set_text_alignment(s_pinas_layer, GTextAlignmentCenter);
  
  // Create date TextLayer
  s_date_layer = text_layer_create(
      GRect(0, 120, bounds.size.w, 60));
 
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorRed);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Set the bitmap onto the layer and add to the window
//   bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
//   layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
 
  // Add Pilipinas text to Window
  layer_add_child(window_layer, text_layer_get_layer(s_pinas_layer));
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  // Add date to Window
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  //Unload date font and TextLayer
  fonts_unload_custom_font(s_date_font);
  text_layer_destroy(s_date_layer);
  
  //Destroy Pinas text Layer
  text_layer_destroy(s_pinas_layer);
  
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_set_background_color(s_main_window, GColorBlue);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
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