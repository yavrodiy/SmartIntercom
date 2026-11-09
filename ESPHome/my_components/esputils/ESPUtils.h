
#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <FTPServer.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>

namespace esphome {
    namespace esputils {

        static const char *const TAG = "esputils";

        inline bool ensure_filesystem_mounted() {
            static bool mounted = false;

            if (!mounted) {
                mounted = LittleFS.begin(false);
                if (!mounted) {
                    ESP_LOGE(TAG, "LittleFS mount failed");
                }
            }

            return mounted;
        }


        // ============================================================
        // FTP SERVER
        // ============================================================

        class ESPUtils : public Component {
        public:
            ESPUtils(const char *username, const char *password)
            : username_(username), password_(password) {}

            float get_setup_priority() const override {
                return setup_priority::AFTER_WIFI;
            }

            void setup() override {
                if (!ensure_filesystem_mounted()) {
                    ESP_LOGE(TAG, "FTP server will not start");
                    return;
                }

                this->server_ = new FTPServer(LittleFS);
                this->server_->begin(this->username_, this->password_);

                ESP_LOGI(TAG, "FTP server started");
            }

            void loop() override {
                if (this->server_ != nullptr) {
                    this->server_->handleFTP();
                }
            }

            void dump_config() override {
                ESP_LOGCONFIG(TAG, "FTP Server:");
                ESP_LOGCONFIG(TAG, "  Filesystem: LittleFS");
            }

            ~ESPUtils() {
                delete this->server_;
            }

        protected:
            FTPServer *server_{nullptr};

            const char *username_;
            const char *password_;
        };


        // ============================================================
        // FILESYSTEM FREE SPACE TEXT SENSOR
        // ============================================================

        class FSInfoSensor : public PollingComponent,
        public text_sensor::TextSensor {
        public:
            FSInfoSensor() : PollingComponent(60000) {}

            float get_setup_priority() const override {
                return setup_priority::AFTER_WIFI;
            }

            void setup() override {
                this->fs_initialized_ = ensure_filesystem_mounted();

                if (!this->fs_initialized_) {
                    this->publish_state("Ошибка");
                }
            }

            void update() override {
                if (!this->fs_initialized_) {
                    this->publish_state("Ошибка");
                    return;
                }

                uint64_t total = LittleFS.totalBytes();
                uint64_t used = LittleFS.usedBytes();

                uint64_t free_bytes = total > used ? total - used : 0;

                // Резерв свободного пространства, как в исходной библиотеке
                constexpr uint64_t CRITICAL_FREE = 16384;

                free_bytes = free_bytes > CRITICAL_FREE
                ? free_bytes - CRITICAL_FREE
                : 0;

                const char *sizes[] = {"B", "KB", "MB", "GB", "TB"};

                double value = static_cast<double>(free_bytes);
                uint8_t unit = 0;

                while (value >= 1024.0 && unit < 4) {
                    value /= 1024.0;
                    unit++;
                }

                char buffer[24];

                snprintf(
                    buffer,
                    sizeof(buffer),
                         "%.2f %s",
                         value,
                         sizes[unit]
                );

                this->publish_state(buffer);
            }

        protected:
            bool fs_initialized_{false};
        };

    }  // namespace esputils
}  // namespace esphome
