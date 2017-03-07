package keisukenishizawa.melodymemo;

import android.app.ProgressDialog;
import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.format.Time;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.text.format.Time;
import android.widget.Toast;

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
    TextView textStat;
    public String filename= "tmp.wav";
    private MediaPlayer mp;
    private MediaRecorder rec;
    /* 録音先のパス */
    String filePath= Environment.getExternalStorageDirectory() + "/MelodyMemo/waves/+filename";
    MidiFile midiFile;
    ImageButton record,stop,play,pause,pitchDetect;
   // ProgressBar progressBar;
   // ProgressDialog progressDialog;
    Time time;

    public double[] pitchStr;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        String target_path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/MelodyMemo/waves";
        File dir = new File(target_path);
        if(!dir.exists()){
            dir.mkdirs();
        }

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

        //progressBar = (ProgressBar) findViewById(R.id.progressBar);

        record = (ImageButton)findViewById(R.id.record);
        stop = (ImageButton)findViewById(R.id.stop);
        play = (ImageButton)findViewById(R.id.play);
        pause = (ImageButton)findViewById(R.id.pause);
        pitchDetect = (ImageButton)findViewById(R.id.pitchDetect);


        record.setVisibility(View.VISIBLE);
        stop.setVisibility(View.VISIBLE);
        play.setVisibility(View.GONE);
        pause.setVisibility(View.GONE);

        time = new Time("Asia/Tokyo");
        textStat = (TextView)findViewById(R.id.textStat);




        record.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                /* ファイルが存在する場合は削除 */
                time.setToNow();
                filename = time.year + "_"+ (time.month+1) + "_" + time.monthDay + "-"
                        +time.hour + "_" + time.minute + "_" + time.second + ".wav";
                filePath = Environment.getExternalStorageDirectory() + "/MelodyMemo/waves/"+filename;
                File wavFile = new File(filePath);
                if (wavFile.exists()) {
                    wavFile.delete();
                }
                wavFile = null;
                try {
                    // time = new Time("Asia/Tokyo");
                    Toast.makeText(getApplicationContext(),"録音開始",Toast.LENGTH_SHORT).show();
                    textStat.setText("録音中");
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
        });


        stop.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                try {
                    textStat.setText("録音停止");
                    Toast.makeText(getApplicationContext(),filename+"で保存しました",Toast.LENGTH_SHORT).show();
                    rec.stop();
                    rec.reset();
                    rec.release();
                    stop.setVisibility(View.GONE);
                    play.setVisibility(View.VISIBLE);
                    //pause.setVisibility(View.VISIBLE);

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });



        play.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                try {
                    textStat.setText(filename+"を再生");
                    mp = new MediaPlayer();
                    mp.setDataSource(filePath);
                    mp.prepare();
                    mp.start();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        pause.setOnClickListener(new View.OnClickListener(){
          @Override
            public void onClick(View v){

              mp.pause();
          }
        });

        pitchDetect.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                try {
                    Toast.makeText(getApplicationContext(),"ピッチ抽出中",Toast.LENGTH_SHORT).show();
                  /*  progressDialog = new ProgressDialog();
                    progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
                    progressDialog.setMessage("処理を実行中しています");
                    progressDialog.setCancelable(true);
                    progressDialog.show();*/
                    pitchDetect();
                    Toast.makeText(getApplicationContext(),"ピッチ抽出完了",Toast.LENGTH_SHORT).show();

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });


        // Example of a call to a native method
       // TextView tv = (TextView) findViewById(R.id.sample_text);
       // tv.setText(stringFromJNI());

        midiMake();

    }//onCreate() finished


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

    public void pitchDetect(){
        pitchStr = new double[waveSizeRead()];
       // pitch(pitchStr,(char)filePath);

    }


    public native String stringFromJNI();
    public native void pitch();
    public native int waveSizeRead();

}
