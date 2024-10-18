import "package:flutter/material.dart";

/// The default theme for the ReVoiceMe app.
///
/// {@category Other}
ThemeData buildThemeData(BuildContext context) => ThemeData(
      colorScheme: ColorScheme.fromSeed(
        seedColor: Colors.lightBlue,
        primary: Colors.lightBlue,
        secondary: Colors.orange,
      ),
      iconTheme: const IconThemeData(size: CustomThemeData.defaultIconSize),
      useMaterial3: true,
      textTheme: Theme.of(context).textTheme.apply(
            fontSizeFactor:
                1.25, // Scales all fonts so that elderly people can read them better
          ),
      sliderTheme:
          SliderThemeData(tickMarkShape: SliderTickMarkShape.noTickMark),
    );

/// Object containing design constants for the ReVoiceMe app.
///
/// {@category Other}
class CustomThemeData {
  /// Uniform default size for corner roundness.
  static const double defaultBorderRadius = 24.0;

  /// Uniform default size for icons.
  static const double defaultIconSize = 30.0;

  /// Uniform default padding size.
  static const double defaultPadding = 16.0;

  /// Uniform small padding size.
  static const double smallPadding = 8.0;

  /// The height of the drawer header.
  static const double drawerHeaderHeight = 64.0;
}
