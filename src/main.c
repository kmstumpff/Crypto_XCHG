#include <pebble.h>

	
enum {
	MRKT_1 = 0,
	MRKT_2 = 1,
	MRKT_3 = 2,
	MRKT_4 = 3,
	MRKT_5 = 4,
	MRKT_6 = 5,
	MRKT_7 = 6,
	MRKT_8 = 7,
};

#define MRKT_KEY_MIN 0
#define MRKT_KEY_MAX 7
	
	// setup functions
void setup_app_message();
void interpret_message_result(AppMessageResult app_message_error);

static Window *window;

static TextLayer *name_layer;
static TextLayer *title_price_layer;
static TextLayer *price_layer;
static TextLayer *title_volume_layer;
static TextLayer *volume_layer;

AppSync sync;

int mrkt_key = MRKT_1;
int mrkt_key_prev = MRKT_1;
const char *mrkt_names[MRKT_KEY_MAX + 1];
int mrkt_ids[MRKT_KEY_MAX + 1];

void setup_names() {
	mrkt_names[MRKT_1] = "LTC/BTC";
	mrkt_names[MRKT_2] = "DOGE/BTC";
	mrkt_names[MRKT_3] = "DOGE/LTC";
	mrkt_names[MRKT_4] = "NMC/BTC";
	mrkt_names[MRKT_5] = "PPC/BTC";
	mrkt_names[MRKT_6] = "PPC/LTC";
	mrkt_names[MRKT_7] = "QRK/BTC";
	mrkt_names[MRKT_8] = "QRK/LTC";
	// Cryptsy Market IDs
	mrkt_ids[MRKT_1] = 3;
	mrkt_ids[MRKT_2] = 132;
	mrkt_ids[MRKT_3] = 135;
	mrkt_ids[MRKT_4] = 29;
	mrkt_ids[MRKT_5] = 28;
	mrkt_ids[MRKT_6] = 125;
	mrkt_ids[MRKT_7] = 71;
	mrkt_ids[MRKT_7] = 126;
}

enum {
	MARKETID = 0x0,	// TUPLE_CSTRING
	PRICE = 0x1,	// TUPLE_CSTRING
	VOLUME = 0x2,	// TUPLE_CSTRING
};

void out_sent_handler(DictionaryIterator *sent, void *context) 
{}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) 
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "PEBBLE: out_failed_handler");
	
	//Get reason
	interpret_message_result(reason);
}

void in_received_handler(DictionaryIterator *iter, void *context) 
{
	//Output?
	Tuple *price_tuple = dict_find(iter, PRICE);
	Tuple *volume_tuple = dict_find(iter, VOLUME);
	if(price_tuple) 
	{
		text_layer_set_text(price_layer, price_tuple->value->cstring);
	}
	if(volume_tuple) 
	{
		text_layer_set_text(volume_layer, volume_tuple->value->cstring);
	}
}

void in_dropped_handler(AppMessageResult reason, void *context) 
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "PEBBLE: in_dropped_handler");
	
	//Get reason
	interpret_message_result(reason);
}

void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	
	name_layer = text_layer_create(GRect(0, 5, 144, 68));
	text_layer_set_text_color(name_layer, GColorWhite);
	text_layer_set_background_color(name_layer, GColorClear);
	text_layer_set_font(name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(name_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(name_layer));
	text_layer_set_text(name_layer, mrkt_names[MRKT_1]);
	
	title_price_layer = text_layer_create(GRect(0, 35, 144, 68));
	text_layer_set_text_color(title_price_layer, GColorWhite);
	text_layer_set_background_color(title_price_layer, GColorClear);
	text_layer_set_font(title_price_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(title_price_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(title_price_layer));
	text_layer_set_text(title_price_layer, "Price");
	
	price_layer = text_layer_create(GRect(0, 58, 144, 68));
	text_layer_set_text_color(price_layer, GColorWhite);
	text_layer_set_background_color(price_layer, GColorClear);
	text_layer_set_font(price_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_text_alignment(price_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(price_layer));
	text_layer_set_text(price_layer, "Loading...");
	
	title_volume_layer = text_layer_create(GRect(0, 88, 144, 68));
	text_layer_set_text_color(title_volume_layer, GColorWhite);
	text_layer_set_background_color(title_volume_layer, GColorClear);
	text_layer_set_font(title_volume_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(title_volume_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(title_volume_layer));
	text_layer_set_text(title_volume_layer, "Volume");
	
	volume_layer = text_layer_create(GRect(0, 111, 144, 68));
	text_layer_set_text_color(volume_layer, GColorWhite);
	text_layer_set_background_color(volume_layer, GColorClear);
	text_layer_set_font(volume_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_text_alignment(volume_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(volume_layer));
	text_layer_set_text(volume_layer, "Loading...");
}

void window_unload(Window *window) {
	app_sync_deinit(&sync);
	text_layer_destroy(name_layer);
	text_layer_destroy(title_price_layer);
	text_layer_destroy(price_layer);
	text_layer_destroy(title_volume_layer);
	text_layer_destroy(volume_layer);
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	if (mrkt_key == MRKT_KEY_MIN) {
		mrkt_key = MRKT_KEY_MAX;
	} else {
		mrkt_key_prev = mrkt_key--;
	}
	text_layer_set_text(name_layer, mrkt_names[mrkt_key]);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "\nfirst: %i\nsecond: %i", mrkt_key, mrkt_key_prev);
	if (mrkt_key != mrkt_key_prev) {
		text_layer_set_text(price_layer, "Loading...");
		text_layer_set_text(volume_layer, "Loading...");
		Tuplet mid_tuple = TupletInteger(MARKETID, mrkt_ids[mrkt_key]);
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);
		dict_write_tuplet(iter, &mid_tuple);
	}else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "skipping");
	}
	
	mrkt_key_prev = mrkt_key;
	app_message_outbox_send();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	if (mrkt_key == MRKT_KEY_MAX) {
		mrkt_key = MRKT_KEY_MIN;
	} else {
		mrkt_key_prev = mrkt_key++;
	}
	text_layer_set_text(name_layer, mrkt_names[mrkt_key]);
}

void config_provider(Window *window)
{
	window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_single_click_handler);
}

void init(void) {
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_set_fullscreen(window, false);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});
	
	const bool animated = true;
	window_stack_push(window, animated);
}

void deinit(void) {
	window_destroy(window);
}

int main(void) {
	setup_names();
	init();
	setup_app_message();
	window_set_click_config_provider(window, (ClickConfigProvider)config_provider);
	app_event_loop();
	deinit();
	return 0;
}


void setup_app_message()
{
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
	
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}



void interpret_message_result(AppMessageResult app_message_error)
{
	if (app_message_error == APP_MSG_OK)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_OK");
	else if (app_message_error == APP_MSG_SEND_TIMEOUT)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_SEND_TIMEOUT");
	else if (app_message_error == APP_MSG_SEND_REJECTED)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_SEND_REJECTED");
	else if (app_message_error == APP_MSG_NOT_CONNECTED)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_NOT_CONNECTED");
	else if (app_message_error == APP_MSG_APP_NOT_RUNNING)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_APP_NOT_RUNNING");
	else if (app_message_error == APP_MSG_INVALID_ARGS)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_INVALID_ARGS");
	else if (app_message_error == APP_MSG_BUSY)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_BUSY");
	else if (app_message_error == APP_MSG_BUFFER_OVERFLOW)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_BUFFER_OVERFLOW");
	else if (app_message_error == APP_MSG_ALREADY_RELEASED)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_ALREADY_RELEASED");
	else if (app_message_error == APP_MSG_CALLBACK_ALREADY_REGISTERED)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_CALLBACK_ALREADY_REGISTERED");
	else if (app_message_error == APP_MSG_CALLBACK_NOT_REGISTERED)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: APP_MSG_CALLBACK_NOT_REGISTERED");
	else
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: Unknown Error");
}
