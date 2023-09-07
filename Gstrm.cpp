#include <gst/gst.h>
#include <iostream>


#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

int
tutorial_main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <in_file> <out_file>" << std::endl;
        return 1;
    }
    const gchar* in_file = argv[1];
    const gchar* out_file = argv[2];
    const gchar* play = "playbin uri=" + *in_file;
    const gchar* convert = "filesrc location=" + *in_file + 
        *" !qtdemux name=demux  demux.audio_0 ! queue ! decodebin ! audioconvert ! audioresample ! mux.  demux.video_0 ! queue ! h264parse ! mux. avimux name=mux ! filesink location = " + *out_file;

    GstElement* pipeline;
    GstBus* bus;
    GstMessage* msg;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Build the pipeline */
    pipeline =
        gst_parse_launch
        (play,                      //Запуск видео
            NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg =
        gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
            (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));

/* See next tutorial for proper error message handling/parsing */
    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
        g_error("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
            "variable set for more details.");
    }

    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    pipeline =
        gst_parse_launch
        (convert,                   //Из mp4 в avi
            NULL);

    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg =
        gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
            (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));

    /* See next tutorial for proper error message handling/parsing */
    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
        g_error("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
            "variable set for more details.");
    }

    /* Free resources */
    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}

int
main(int argc, char* argv[])
{
#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
    return gst_macos_main(tutorial_main, argc, argv, NULL);
#else
    return tutorial_main(argc, argv);
#endif
}