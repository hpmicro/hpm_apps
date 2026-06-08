/**
 * @file watch_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "watch_gen.h"

#if LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

lv_obj_t *home = NULL;
lv_obj_t *home_layer2 = NULL;
lv_obj_t *home_layer3 = NULL;
lv_obj_t *home_layer4 = NULL;
lv_obj_t *home_layer5 = NULL;
lv_obj_t *home_layer6 = NULL;
lv_obj_t *home_layer7 = NULL;
lv_obj_t *home_layer8 = NULL;

/*----------------
 * Fonts
 *----------------*/

lv_font_t *SemiBold_90;
extern lv_font_t SemiBold_90_data;
lv_font_t *SemiBold_70;
extern lv_font_t SemiBold_70_data;
lv_font_t *SemiBold_50;
extern lv_font_t SemiBold_50_data;
lv_font_t *NotoSeri_50;
extern lv_font_t NotoSeri_50_data;
lv_font_t *NotoSeri_30;
extern lv_font_t NotoSeri_30_data;
lv_font_t *SemiBold_30;
extern lv_font_t SemiBold_30_data;

/*----------------
 * Images
 *----------------*/

const void *bk;
extern const lv_image_dsc_t home_bk;
const void *needle;
extern const void *needle_data;
const void *oil_normal;
extern const void *oil_normal_data;
const void *oil_low;
extern const void *oil_low_data;
const void *anquandai;
extern const void *anquandai_data;
const void *fadongji;
extern const void *fadongji_data;
const void *shousha;
extern const void *shousha_data;
const void *jinguangdeng;
extern const void *jinguangdeng_data;
const void *yuanguangdeng;
extern const void *yuanguangdeng_data;
const void *wudeng;
extern const void *wudeng_data;
const void *youliang;
extern const void *youliang_data;
const void *youzhuan;
extern const void *youzhuan_data;
const void *zuozhuan;
extern const void *zuozhuan_data;
const void *shuiwen_high;
extern const void *shuiwen_high_data;
const void *shuiwen_low;
extern const void *shuiwen_low_data;

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

lv_subject_t subject_speed_value;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void watch_init_gen(const char *asset_path)
{
    char buf[256];

    /*----------------
     * Fonts
     *----------------*/

    /* get font 'SemiBold_90' from a C array */
    SemiBold_90 = &SemiBold_90_data;
    /* get font 'SemiBold_70' from a C array */
    SemiBold_70 = &SemiBold_70_data;
    /* get font 'SemiBold_50' from a C array */
    SemiBold_50 = &SemiBold_50_data;
    /* get font 'NotoSeri_50' from a C array */
    NotoSeri_50 = &NotoSeri_50_data;
    /* get font 'NotoSeri_30' from a C array */
    NotoSeri_30 = &NotoSeri_30_data;
    /* get font 'SemiBold_30' from a C array */
    SemiBold_30 = &SemiBold_30_data;

    /*----------------
     * Images
     *----------------*/
    bk = &home_bk;
    needle = &needle_data;
    oil_normal = &oil_normal_data;
    oil_low = &oil_low_data;
    anquandai = &anquandai_data;
    fadongji = &fadongji_data;
    shousha = &shousha_data;
    jinguangdeng = &jinguangdeng_data;
    yuanguangdeng = &yuanguangdeng_data;
    wudeng = &wudeng_data;
    youliang = &youliang_data;
    youzhuan = &youzhuan_data;
    zuozhuan = &zuozhuan_data;
    shuiwen_high = &shuiwen_high_data;
    shuiwen_low = &shuiwen_low_data;

    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&subject_speed_value, 0);
    lv_subject_set_min_value_int(&subject_speed_value, 0);
    lv_subject_set_max_value_int(&subject_speed_value, 240);

    /*----------------
     * Translations
     *----------------*/

#if LV_USE_XML
    /* Register widgets */

    /* Register fonts */
    lv_xml_register_font(NULL, "SemiBold_90", SemiBold_90);
    lv_xml_register_font(NULL, "SemiBold_70", SemiBold_70);
    lv_xml_register_font(NULL, "SemiBold_50", SemiBold_50);
    lv_xml_register_font(NULL, "NotoSeri_50", NotoSeri_50);
    lv_xml_register_font(NULL, "NotoSeri_30", NotoSeri_30);
    lv_xml_register_font(NULL, "SemiBold_30", SemiBold_30);

    /* Register subjects */
    lv_xml_register_subject(NULL, "subject_speed_value", &subject_speed_value);

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "bk", bk);
    lv_xml_register_image(NULL, "needle", needle);
    lv_xml_register_image(NULL, "oil_normal", oil_normal);
    lv_xml_register_image(NULL, "oil_low", oil_low);
    lv_xml_register_image(NULL, "anquandai", anquandai);
    lv_xml_register_image(NULL, "fadongji", fadongji);
    lv_xml_register_image(NULL, "shousha", shousha);
    lv_xml_register_image(NULL, "jinguangdeng", jinguangdeng);
    lv_xml_register_image(NULL, "yuanguangdeng", yuanguangdeng);
    lv_xml_register_image(NULL, "wudeng", wudeng);
    lv_xml_register_image(NULL, "youliang", youliang);
    lv_xml_register_image(NULL, "youzhuan", youzhuan);
    lv_xml_register_image(NULL, "zuozhuan", zuozhuan);
    lv_xml_register_image(NULL, "shuiwen_high", shuiwen_high);
    lv_xml_register_image(NULL, "shuiwen_low", shuiwen_low);
#endif

#if LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manaully from XML using lv_xml_create() */
    /* To allow screens to reference each other, create them all before calling the sceen create functions */
    home = lv_obj_create(NULL);

    home_create();
#endif
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/
