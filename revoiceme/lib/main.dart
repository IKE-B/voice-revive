import "package:flutter/material.dart";
import "package:flutter_bloc/flutter_bloc.dart";
import "package:revoiceme/core/routers/router.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_bloc.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_event.dart";

void main() {
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
