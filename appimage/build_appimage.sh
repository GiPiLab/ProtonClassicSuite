#!/bin/bash

#Build ProtonClassicSuite appimage with linuxdeployqt. Put ProtonClassicSuite executable in $APPIMAGE_FOLDER/usr/bin before


LINUXDEPLOYQT="$HOME/progs/linuxdeployqt-continuous-x86_64.AppImage"
APPIMAGE_FOLDER="appimage"
GRAPHVIZPATH=/usr/lib/x86_64-linux-gnu/graphviz

#Update graphviz from system libs

cp -fv $GRAPHVIZPATH/libgvplugin_core.so.6.0.0 $GRAPHVIZPATH/libgvplugin_dot_layout.so.6.0.0 $GRAPHVIZPATH/libgvplugin_gd.so.6.0.0 $GRAPHVIZPATH/libgvplugin_pango.so.6.0.0 $GRAPHVIZPATH/libgvplugin_rsvg.so.6.0.0 $APPIMAGE_FOLDER/usr/lib/graphviz

#Build appimage
$LINUXDEPLOYQT $APPIMAGE_FOLDER/usr/share/applications/ProtonClassicSuite.desktop -extra-plugins=iconengines,platforms,platformthemes,wayland-decoration-client,wayland-graphics-integration-client,wayland-shell-integration -executable=$APPIMAGE_FOLDER/usr/lib/graphviz/libgvplugin_core.so -executable=$APPIMAGE_FOLDER/usr/lib/graphviz/libgvplugin_dot_layout.so  -executable=$APPIMAGE_FOLDER/usr/lib/graphviz/libgvplugin_gd.so -executable=$APPIMAGE_FOLDER/usr/lib/graphviz/libgvplugin_pango.so -executable=$APPIMAGE_FOLDER/usr/lib/graphviz/libgvplugin_rsvg.so  -appimage
