vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO btzy/nativefiledialog-extended
    REF v${VERSION}
    SHA512 4ec3e174a90354c524d9be2776422740f80b73021df94e1942e60ab4310995245554f83097b9b2dcca04d016a8548d3fc0760f73daf724c5c3d72c15cf776bed
    HEAD_REF master
)

# Overlay override: on Linux, use the GTK3 backend instead of the
# xdg-desktop-portal backend. The portal backend spawns the dialog as a
# separate out-of-process window (xdg-desktop-portal-kde/gnome) whose
# placement is decided entirely by the window manager, with no "center on
# parent" option in the portal protocol — it was appearing pinned to the
# screen's top-left corner regardless of the parent window handle NFD
# passes it. GTK3's file chooser is a normal transient-for X11 window, so
# the window manager centers it over the app window like any other dialog.
# This option only affects the Linux build path (Windows/macOS ignore it).
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DNFD_BUILD_TESTS=OFF
        -DNFD_PORTAL=OFF
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME nfd CONFIG_PATH lib/cmake/nfd)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_copy_pdbs()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
