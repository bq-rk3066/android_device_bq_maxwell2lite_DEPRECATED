#
# Copyright (C) 2012 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

add_lunch_combo cm_maxwell2lite-userdebug

# Run device patches on the tree.
sh device/bq/maxwell2lite/patches/apply.sh

# HDMI files and patches.
cp -r device/bq/maxwell2lite/hdmi/files/. packages/apps/Settings/
sh device/bq/maxwell2lite/hdmi/patches/apply.sh
