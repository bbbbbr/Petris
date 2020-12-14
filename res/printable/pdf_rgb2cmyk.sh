# https://stackoverflow.com/questions/31591554/embed-icc-color-profile-in-pdf
# https://ghostscript.com/doc/9.27/GS9_Color_Management.pdf
#
# Recommendations from a 9.15 version of gs
# gs -o cmyk-doc.pdf      \
#   -sDEVICE=pdfwrite    \
#   -dOverrideICC=true   \
#   -sDefaultCMYKProfile=/path/to/mycmykprofile.icc \
#   -sOutputICCProfile=/path/to/mydeviceprofile.icc \
#   -dRenderIntent=3     \
#   -dDeviceGrayToK=true \
#    input-doc.pdf
gs -dSAFER -dBATCH -dNOPAUSE -dNOCACHE -sDEVICE=pdfwrite -dAutoRotatePages=/None -sColorConversionStrategy=CMYK -dProcessColorModel=/DeviceCMYK -sOutputFile=cartridge_sticker_petris_print_cmyk.pdf cartridge_sticker_petris_print_rgb.pdf
