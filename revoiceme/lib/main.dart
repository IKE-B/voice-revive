import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:record/record.dart';
import 'package:audioplayers/audioplayers.dart';
import 'package:path_provider/path_provider.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  late AudioPlayer audioPlayer;
  late AudioRecorder audioRecorder;
  bool isRecording = false;
  String audioPath = '';

  @override
  void initState() {
    audioRecorder = AudioRecorder();
    audioPlayer = AudioPlayer();
    super.initState();
  }

  @override
  void dispose() {
    audioRecorder.dispose();
    audioPlayer.dispose();
    super.dispose();
  }

  Future<void> startRecording() async{
    try{
      if(await audioRecorder.hasPermission()){
        final tempDir = await getTemporaryDirectory();
        await audioRecorder.start(const RecordConfig(), path: '${tempDir.path}/temp-audio.m4a');
        setState(() {
          isRecording = true;
        });
      }
    }
    catch(e){
      if (kDebugMode) {
        print('Error Start Recording: $e');
      }
    }
  }

  Future<void> stopRecording() async{
    try{
      String? path = await audioRecorder.stop();
      setState(() {
        isRecording = false;
        audioPath = path!;
      });
    }
    catch(e){
      if (kDebugMode) {
        print('Error Stopping Recording: $e');
      }
    }
  }

  Future<void> playRecording() async{
    try{
      Source urlSource = UrlSource(audioPath);
      await audioPlayer.play(urlSource);
    }
    catch(e){
      if (kDebugMode) {
        print('Error playing Recording: $e');
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('ReVoiceMe')
      ),
      body: Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: Column(
          // Column is also a layout widget. It takes a list of children and
          // arranges them vertically. By default, it sizes itself to fit its
          // children horizontally, and tries to be as tall as its parent.
          //
          // Column has various properties to control how it sizes itself and
          // how it positions its children. Here we use mainAxisAlignment to
          // center the children vertically; the main axis here is the vertical
          // axis because Columns are vertical (the cross axis would be
          // horizontal).
          //
          // TRY THIS: Invoke "debug painting" (choose the "Toggle Debug Paint"
          // action in the IDE, or press "p" in the console), to see the
          // wireframe for each widget.
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            if(isRecording)
              const Text(
                'Recording in Progress',
                style: TextStyle(
                  fontSize: 20
                )
              ),
            ElevatedButton(
              onPressed: isRecording ? stopRecording : startRecording,
              child: isRecording ? const Text('Stop Recording') : const Text('Start Recording')
            ),

            const SizedBox(
              height: 25,
            ),

            if(!isRecording && audioPath != '')
            ElevatedButton(
              onPressed: playRecording,
              child: const Text('Play Recording')
            ),
          ]
        ),
      ),
    );
  }
}
