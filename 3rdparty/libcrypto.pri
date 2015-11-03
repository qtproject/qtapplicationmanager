
unix:packagesExist("'libcrypto >= 1.0.1'") {
  INCLUDEPATH += $$system($$pkgConfigExecutable() --variable=includedir libcrypto)
} else {
  DEFINES += $$AM_LIBCRYPTO_DEFINES
  INCLUDEPATH += $$AM_LIBCRYPTO_INCLUDES
}
