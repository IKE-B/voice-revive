import "package:flutter/material.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";

/// A widget to apply uniformly sized padding in the app where needed.
/// 
/// {@category Widget}
class SmallPadder extends StatelessWidget {
  /// Creates a new [SmallPadder] instance.
  const SmallPadder({super.key});

  @override
  Widget build(BuildContext context) =>
      const SizedBox(width: CustomThemeData.smallPadding, height: CustomThemeData.smallPadding);
}
