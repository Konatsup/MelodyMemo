package keisukenishizawa.melodymemo;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.leff.midi.MidiFile;
import com.leff.midi.MidiTrack;
import com.leff.midi.event.MidiEvent;
import com.leff.midi.event.meta.Tempo;
import com.leff.midi.event.meta.TimeSignature;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("pitch");
        System.loadLibrary("waveSizeRead");
    }

    MidiFile midiFile;

    Button record,stop,play;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        MidiEvent event = new MidiEvent(100, 100) {
            @Override
            protected int getEventSize() {
                return 0;
            }

            @Override
            public int compareTo(MidiEvent midiEvent) {
                return 0;
            }
        };

        // ボタンを設定
        Button button = (Button)findViewById(R.id.button);
        record = (Button)findViewById(R.id.record);
        stop = (Button)findViewById(R.id.stop);
        play = (Button)findViewById(R.id.play);

        // リスナーをボタンに登録
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
             //   Log.d("debug", "erroraaaaaaaaaaa");
                wavPlay();
            }
        });


        // Example of a call to a native method
       // TextView tv = (TextView) findViewById(R.id.sample_text);
       // tv.setText(stringFromJNI());

        midiMake();
    }

    // hello_world.wav のサンプリングレート
    private static final int SamplingRate = 32000;

    private MediaPlayer mp;
    private MediaRecorder rec;
    /* 録音先のパス */
    static final String filePath = Environment.getExternalStorageDirectory() + "/sample.wav";

/* 略 */


    private void wavPlay(){
        InputStream input;
        byte[] wavData;

        try {
            // wavを読み込む
            input = getResources().openRawResource(R.raw.how_are_you);
            wavData = new byte[input.available()];
            input.read(wavData);
            input.close();

            // バッファサイズの計算
            int bufSize = android.media.AudioTrack.getMinBufferSize(
                    SamplingRate, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT);

            // MODE_STREAM にてインスタンス生成
            AudioTrack audioTrack = new AudioTrack(
                    AudioManager.STREAM_MUSIC, SamplingRate, AudioFormat.CHANNEL_OUT_MONO,
                    AudioFormat.ENCODING_PCM_16BIT, bufSize, AudioTrack.MODE_STREAM);

            // 再生
            audioTrack.play();

            //audioTrack.write(wavData, 0, wavData.length);
            // ヘッダ44byteをオミット
            audioTrack.write(wavData, 44, wavData.length-44);

        } catch (IOException e) {
            e.printStackTrace();
            Log.d("debug", "error");
        }
    }
    public void startPlay(View v) {
        //mp = MediaPlayer.create(this, R.raw.sample);
        try {
            mp = new MediaPlayer();
            mp.setDataSource(filePath);
            mp.prepare();
            mp.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void startRecord(View v) {
        /* ファイルが存在する場合は削除 */
        File wavFile = new File(filePath);
        if (wavFile.exists()) {
            wavFile.delete();
        }
        wavFile = null;
        try {
            rec = new MediaRecorder();
            rec.setAudioSource(MediaRecorder.AudioSource.MIC);
            rec.setOutputFormat(MediaRecorder.OutputFormat.DEFAULT);
            rec.setAudioEncoder(MediaRecorder.AudioEncoder.DEFAULT);
            rec.setOutputFile(filePath);

            rec.prepare();
            rec.start();
        } catch(Exception e){
            e.printStackTrace();
        }
    }

    public void stopRecord(View v) {
        try {
            rec.stop();
            rec.reset();
            rec.release();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void midiMake(){
        MidiTrack tempoTrack = new MidiTrack();
        MidiTrack noteTrack = new MidiTrack();

// 2. Add events to the tracks
// Track 0 is the tempo map
        TimeSignature ts = new TimeSignature();
        ts.setTimeSignature(4, 4, TimeSignature.DEFAULT_METER, TimeSignature.DEFAULT_DIVISION);

        Tempo tempo = new Tempo();
        tempo.setBpm(228);

        tempoTrack.insertEvent(ts);
        tempoTrack.insertEvent(tempo);

// Track 1 will have some notes in it
        final int NOTE_COUNT = 80;

        for(int i = 0; i < NOTE_COUNT; i++)
        {
            int channel = 0;
            int pitch = 1 + i;
            int velocity = 100;
            long tick = i * 480;
            long duration = 120;

            noteTrack.insertNote(channel, pitch, velocity, tick, duration);
        }

// 3. Create a MidiFile with the tracks we created
        List<MidiTrack> tracks = new ArrayList<MidiTrack>();
        tracks.add(tempoTrack);
        tracks.add(noteTrack);

        MidiFile midi = new MidiFile(MidiFile.DEFAULT_RESOLUTION, (ArrayList<MidiTrack>) tracks);

// 4. Write the MIDI data to a file
        File output = new File("exampleout.mid");
        try
        {
            midi.writeToFile(output);
        }
        catch(IOException e)
        {
            System.err.println(e);
        }

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

    public void pitchDetect(View v){



    }


    public native String stringFromJNI();
    public native void pitch();
    public native double waveSizeRead();

}
