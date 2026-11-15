// The gray rule of the theme and its limits. theme/light-theme.m and the launcher app both
// include this file, so that the preview in the app always matches the theme.
#include <math.h>

// Black text at 0.18 keeps a contrast of about 5.6:1 on this gray.
#define SURFACE_MAX_MIN 0.65
#define GAMMA_MIN 0.5
#define GAMMA_MAX 1.5
// The selected row in a list is this fraction of the lightest gray.
#define SELECTED_ROW_WHITE 0.95
// Stock grays from this level up are text, glyphs and some control bodies, such as knobs.
#define FOREGROUND_MIN 0.55
// The stock gray of buttons, the darkest surface that often holds text.
#define STOCK_TEXT_BACKGROUND 0.26
// The contrast that text keeps on that surface at every setting.
#define TEXT_CONTRAST 4.5
// Translucent text grays at or above this opacity get more opacity when they need it. Lighter
// overlays, such as hover fills, keep their opacity.
#define TEXT_ALPHA_MIN 0.3
// The preferences domain of the launcher app, which is also its bundle identifier.
#define SETTINGS_DOMAIN_NAME "io.github.karlingen.i-see-logic-pro"

static inline double toLinear(double level) {
   return level <= 0.04045 ? level / 12.92 : pow((level + 0.055) / 1.055, 2.4);
}

static inline double toSRGB(double linear) {
   return linear <= 0.0031308 ? linear * 12.92 : 1.055 * pow(linear, 1 / 2.4) - 0.055;
}

/**
 * Maps a stock gray level to the themed gray level. The darkest stock gray becomes surfaceMax,
 * and white becomes black.
 *
 * @param white double - the stock gray level from 0 to 1
 * @param surfaceMax double - the lightest themed gray
 * @param gamma double - the curve between the two ends
 * @return double - the themed gray level
 */
static inline double textBackground(double surfaceMax, double gamma) {
   return surfaceMax * pow(1 - STOCK_TEXT_BACKGROUND, gamma);
}

// The lightest text gray with TEXT_CONTRAST on a themed button.
static inline double textLimit(double surfaceMax, double gamma) {
   return toSRGB(fmax(0, (toLinear(textBackground(surfaceMax, gamma)) + 0.05) / TEXT_CONTRAST - 0.05));
}

static inline double mapGrayLevel(double white, double surfaceMax, double gamma) {
   return surfaceMax * pow(1 - white, gamma);
}

/**
 * Maps the gray level of a color that is only for text. At the darker settings, the curve puts
 * secondary text close to the surfaces, so the level is at most textLimit.
 *
 * @param white double - the stock gray level from 0 to 1
 * @param surfaceMax double - the lightest themed gray
 * @param gamma double - the curve between the two ends
 * @return double - the themed gray level
 */
static inline double mapTextLevel(double white, double surfaceMax, double gamma) {
   return fmin(mapGrayLevel(white, surfaceMax, gamma), textLimit(surfaceMax, gamma));
}

/**
 * Returns the opacity of a translucent text gray. The surface shows through translucent text,
 * so the opacity rises until the text on a themed button reaches TEXT_CONTRAST, or 1.
 *
 * @param white double - the stock gray level from 0 to 1
 * @param alpha double - the stock opacity from 0 to 1
 * @param surfaceMax double - the lightest themed gray
 * @param gamma double - the curve between the two ends
 * @return double - the new opacity
 */
static inline double mapTextAlpha(double white, double alpha, double surfaceMax, double gamma) {
   double back = textBackground(surfaceMax, gamma);
   double text = mapTextLevel(white, surfaceMax, gamma);

   if (white < FOREGROUND_MIN || alpha < TEXT_ALPHA_MIN || alpha >= 1 || back <= text) {
      return alpha;
   }
   return fmin(1, fmax(alpha, (back - textLimit(surfaceMax, gamma)) / (back - text)));
}
