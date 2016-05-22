#include <pebble.h>
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_bglayer, *s_weather_layer;
static GFont s_time_font, s_date_font, s_weather_font;
static BitmapLayer *s_background_layer;
static BitmapLayer *s_panda_layer;
static GBitmap *s_background_bitmap;
static GBitmap *s_panda_bitmap;

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
  strftime(date_buffer, sizeof(date_buffer), "%b.%d.%a", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  // Show the date
  text_layer_set_text(s_date_layer, date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
  
    // Send the message!
    app_message_outbox_send();
  }
  
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UNRULY_ORIGINAL);
  
  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(GRect(32, 0, 110, 90));
  
  // Set the correct compositing mode
  bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  
  // Create GBitmap
  s_panda_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PANDA_WAVE);
  
  // Create BitmapLayer to display the GBitmap
  s_panda_layer = bitmap_layer_create(GRect(-20, 62, 90, 50));
  
  // Set the correct compositing mode
  bitmap_layer_set_compositing_mode(s_panda_layer, GCompOpSet);

  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CRACKED_45));
  
  //Create date font
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTUMONO_18));
  
  //Create weather font
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_16));

  // Create temperature Layer
  s_weather_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(115, 110), bounds.size.w, 22));
  
  text_layer_set_background_color(s_weather_layer, GColorBlue);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "...");
  text_layer_set_font(s_weather_layer, s_weather_font);
 
  // Create Time TextLayer
  s_time_layer = text_layer_create(
      GRect(0, 60, bounds.size.w, 50));
 
  text_layer_set_text_color(s_time_layer, GColorBlue);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Create BG TextLayer
  s_bglayer = text_layer_create(
      GRect(0, 0, bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_bglayer, GColorWhite);
  
  // Create Date TextLayer
  s_date_layer = text_layer_create(
      GRect(0, 135, bounds.size.w, 30));
 
  text_layer_set_text_color(s_date_layer, GColorBlue);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
 
  //Add flag colors to Window
  layer_add_child(window_layer, text_layer_get_layer(s_bglayer));
//   layer_add_child(window_layer, text_layer_get_layer(s_red_layer));
  
  // Add date to Window
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  bitmap_layer_set_bitmap(s_panda_layer, s_panda_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_panda_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(s_panda_bitmap);
  
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  //Unload date font and TextLayer
  fonts_unload_custom_font(s_date_font);
  text_layer_destroy(s_date_layer);
  
  // Destroy weather elements
  fonts_unload_custom_font(s_weather_font);
  text_layer_destroy(s_weather_layer);
  
  //Destroy flag text Layer
  text_layer_destroy(s_bglayer);
//   text_layer_destroy(s_red_layer);

}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];

  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);
  
  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d\u00B0", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
  }
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_set_background_color(s_main_window, GColorClear);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  
  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
  
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
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