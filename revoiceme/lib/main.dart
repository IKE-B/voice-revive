import "package:flutter/material.dart";
import "package:flutter_bloc/flutter_bloc.dart";
import "package:revoiceme/core/routers/router.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_bloc.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_event.dart";
import "package:revoiceme/core/utils/juce_interface/ffi_audioprocessor.dart";

void main() {

  initLibrary();

  /*updateValues(0.0, 50.0, 250.0, 0.0, 3.0, false, false,
               50.0, 250.0, 0.0, 3.0, false, false, false,
               50.0, 250.0, 0.0, 3.0, false, false, false,
               50.0, 250.0, 0.0, 3.0, false, false, false,
               400.0, 2000.0, 20.0, 20000.0, 750.0, 0.0, 1.0, 12.0, 12.0);*/

  runApp(const ReVoiceMe());
}

/// The main widget of the ReVoiceMe app.
///
/// {@category Other}
class ReVoiceMe extends StatefulWidget {
  /// Creates a new [ReVoiceMe] instance.
  const ReVoiceMe({super.key});

  @override
  State<ReVoiceMe> createState() => _ReVoiceMeState();
}

class _ReVoiceMeState extends State<ReVoiceMe> {
  VoiceChangeDemo voiceChangeDemo = VoiceChangeDemo();

  @override
  void initState() {
    super.initState();
    // initialize the voice change demo (global state)
    voiceChangeDemo.add(Init());
    debugPrint("VoiceChangeDemo initialized");
  }

  @override
  void dispose() {
    // dispose the voice change demo (global state)
    voiceChangeDemo.add(Dispose());
    super.dispose();
  }

  @override
  Widget build(BuildContext context) => BlocProvider<VoiceChangeDemo>(
        create: (BuildContext context) => voiceChangeDemo,
        child: MaterialApp.router(
          title: "ReVoiceMe",
          theme: buildThemeData(context),
          routerConfig: router,
        ),
      );
}
