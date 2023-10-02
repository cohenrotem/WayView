%QT_DIR%\bin\rcc.exe window_QT.qrc -o qrc_window_QT.cpp
::Note: After qrc_window_QT.cpp is generated, we have to manually add #include "pch.h" to the generated file.
pause