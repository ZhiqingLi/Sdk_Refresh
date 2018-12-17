#include "music.h"

//APP/APK上的按键的消息

app_result_e music_apk_key_play(uint32 data1, uint32 data2)
{
    //TODO
    //关闭子窗口(相当于点击了子窗口的X)
    app_result_e result = RESULT_NULL;

    //调用主窗口上的按键处理
    g_neednot_song_tts_play = TRUE;
    result = play_key_play();
    g_neednot_song_tts_play = FALSE;

    return result;
}

app_result_e music_apk_key_pause(uint32 data1, uint32 data2)
{
    //TODO
    //关闭子窗口(相当于点击了子窗口的X)
    app_result_e result = RESULT_NULL;

    //调用主窗口上的按键处理
    g_neednot_song_tts_play = TRUE;
    result = play_key_play();
    g_neednot_song_tts_play = FALSE;

    return result;
}

app_result_e music_apk_key_prev(uint32 data1, uint32 data2)
{
    //TODO
    //关闭子窗口(相当于点击了子窗口的X)
    app_result_e result = RESULT_NULL;

    //调用主窗口上的按键处理
    g_neednot_song_tts_play = TRUE;
    result = play_key_prev();
    g_neednot_song_tts_play = FALSE;

    return result;
}

app_result_e music_apk_key_next(uint32 data1, uint32 data2)
{
    //TODO
    //关闭子窗口(相当于点击了子窗口的X)
    app_result_e result = RESULT_NULL;

    //调用主窗口上的按键处理
    g_neednot_song_tts_play = TRUE;
    result = play_key_next();
    g_neednot_song_tts_play = FALSE;

    return result;
}
