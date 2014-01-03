/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_LIGHTS_INTERFACE_H
#define ANDROID_LIGHTS_INTERFACE_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <cutils/log.h>
#include <hardware/hardware.h>

__BEGIN_DECLS

#if PLATFORM_SDK_VERSION >= 16  // for platform version >= android 4.1.1 
#define LOGV(fmt,args...) ALOGV(fmt,##args)
#define LOGD(fmt,args...) ALOGD(fmt,##args)
#define LOGI(fmt,args...) ALOGI(fmt,##args)
#define LOGW(fmt,args...) ALOGW(fmt,##args)
#define LOGE(fmt,args...) ALOGE(fmt,##args)
#endif

#define LIGHTS_HARDWARE_MODULE_ID   "lights"
#define LIGHT_ID_BACKLIGHT          "backlight"
#define LIGHT_FLASH_NONE            0
#define LIGHT_FLASH_TIMED           1

struct light_state_t {
    unsigned int color;
    int flashMode;
    int flashOnMS;
    int flashOffMS;
};

struct light_module_t {
    struct hw_module_t common;
};

struct light_device_t {
    struct hw_device_t common;

    int (*set_light)(struct light_device_t* dev,
            struct light_state_t const* state);
};

__END_DECLS

#endif
