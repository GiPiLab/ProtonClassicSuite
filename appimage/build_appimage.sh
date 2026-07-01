#!/bin/bash

~/progs/linuxdeployqt-continuous-x86_64.AppImage appimage/usr/share/applications/ProtonClassicSuite.desktop -extra-plugins=iconengines,platforms,platformthemes,wayland-decoration-client,wayland-graphics-integration-client,wayland-shell-integration -executable=appimage/usr/lib/graphviz/libgvplugin_core.so -executable=appimage/usr/lib/graphviz/libgvplugin_dot_layout.so  -executable=appimage/usr/lib/graphviz/libgvplugin_gd.so -executable=appimage/usr/lib/graphviz/libgvplugin_pango.so -executable=appimage/usr/lib/graphviz/libgvplugin_rsvg.so  -appimage
