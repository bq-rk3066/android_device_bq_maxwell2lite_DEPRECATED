# Boot animation
TARGET_SCREEN_HEIGHT := 600
TARGET_SCREEN_WIDTH := 1024

# Inherit some common CyanogenMod stuff
$(call inherit-product, vendor/cm/config/common_full_tablet_wifionly.mk)

# Inherit device configuration
$(call inherit-product, device/bq/maxwell2lite/maxwell2lite.mk)

# Device identifier. This must come after all inclusions
PRODUCT_DEVICE := maxwell2lite
PRODUCT_NAME := cm_maxwell2lite
PRODUCT_BRAND := bq
PRODUCT_MODEL := bq Maxwell 2 Lite
PRODUCT_MANUFACTURER := Rockchip

# Set product name
PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=bq_Maxwell2Lite
