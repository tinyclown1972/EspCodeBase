#ifndef _INCLUDE_SIMPLE_GUI_CONFIG_H_
#define _INCLUDE_SIMPLE_GUI_CONFIG_H_
#ifdef CONFIG_SIMPLE_GUI_ENABLE
//=======================================================================//
//= Used for SimpleGUI virtual SDK.                                     =//
//=======================================================================//
#ifdef _SIMPLE_GUI_ENCODE_TEXT_
    #define _SIMPLE_GUI_ENCODE_TEXT_SRC_ ("UTF-8")
    #define _SIMPLE_GUI_ENCODE_TEXT_DEST_ ("GB2312")
#endif // _SIMPLE_GUI_ENCODE_TEXT_

//=======================================================================//
//= Used for SimpleGUI interface.                                       =//
//=======================================================================//
// #define _SIMPLE_GUI_ENABLE_DYNAMIC_MEMORY_

#endif // _INCLUDE_SIMPLE_GUI_CONFIG_H_
#endif