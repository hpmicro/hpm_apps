#ifndef LV_APP_CONF_H
#define LV_APP_CONF_H

/* Use full rendering mode for better compatibility */
#undef LV_USE_HPM_MODE_DIRECT
#define LV_USE_HPM_MODE_DIRECT     1

/* Disable PDMA flush for simplicity */
#undef LV_USE_HPM_PDMA_FLUSH
#define LV_USE_HPM_PDMA_FLUSH      1

#undef LV_USE_HPM_PDMA_WAIT_VSYNC
#define LV_USE_HPM_PDMA_WAIT_VSYNC 1

/* Enable observer pattern for data binding */
#undef LV_USE_OBSERVER
#define LV_USE_OBSERVER 1

#undef LV_USE_SUBJECT
#define LV_USE_SUBJECT 1

/* Enable object naming to avoid compilation errors */
#undef LV_USE_OBJ_NAME
#define LV_USE_OBJ_NAME 1

/* Performance optimization settings */
#undef LV_DEF_REFR_PERIOD
#define LV_DEF_REFR_PERIOD  32      /* 30Hz refresh rate for smooth animations */

#ifdef ESP_PLATFORM
#undef ESP_PLATFORM
#endif
/* Memory configuration for complex UI - defined in lv_conf.h */

#endif