package com.example.ua2efindedges;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

public class UA2EFindEdges extends Activity {
	protected ImageView imageView = null;
	private final String tag = "UA2EFindEdges";
	private Button btnAcquire;
	private Button btnFindEdges;
	
	public native int converttogray(Bitmap bitmapcolor, Bitmap bitmapgray);
	public native int detectededges(Bitmap bitmapgray, Bitmap bitmapedges);
	
	static{
		System.loadLibrary("ua2efindedges");
	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ua2efindedges_main);
        btnAcquire = (Button) findViewById(R.id.AcquireImage);
        btnAcquire.setOnClickListener(new View.OnClickListener(){
        	@Override
        	public void onClick(View v){
        		try{
        			Intent action = new Intent("android.media.action.IMAGE_CAPTURE");
        			startActivityForResult(action, 1);
        		}catch(Exception e){
        			Log.e(tag, "Error occurred [ " + e.getMessage() + " ]");
        		}
        	}
        });
        
        btnFindEdges = (Button) findViewById(R.id.FindEdges);
        btnFindEdges.setOnClickListener(new View.OnClickListener(){
        	@Override
        	public void onClick(View v){
        		try{
        			UA2EFindEdgesApp app = (UA2EFindEdgesApp) getApplication();
        			Bitmap b = app.getB();
        			int width = b.getWidth();
        			int height = b.getHeight();
        			Bitmap bg = Bitmap.createBitmap(width, height, Config.ALPHA_8);
        			Bitmap be = Bitmap.createBitmap(width, height, Config.ALPHA_8);
        			converttogray(b, bg);
        			detectededges(bg, be);
        			app.setB(be);
        			imageView.setImageBitmap(be);
        			btnFindEdges.setVisibility(View.GONE);
        		}catch(Exception e){
        			Log.e(tag, "Error occurred [ " + e.getMessage() + " ]");
        		}
        	}
        });
        
        imageView = (ImageView) findViewById(R.id.PictureFrame);
        UA2EFindEdgesApp app = (UA2EFindEdgesApp) getApplication();
        Bitmap b = app.getB();
        if(b != null){
        	imageView.setImageBitmap(b);
        }
        else{
      	 btnFindEdges.setVisibility(View.GONE);
        }
    }
    
    protected void onActivityResult(int requestCode, int resultCode, Intent data){
    	try{
    		if(requestCode == 1){
    			if(resultCode == RESULT_OK){
    				UA2EFindEdgesApp app = (UA2EFindEdgesApp) getApplication();
    				Bitmap b = app.getB();
    				if(b != null){
    					b.recycle();
    				}
    				
    				b = (Bitmap) data.getExtras().get("data");
    				app.setB(b);
    				if (b != null){
    					imageView.setImageBitmap(b);
    					btnFindEdges.setVisibility(View.VISIBLE);
    				}
    			}
    		}
    	}catch(Exception e){
    		Log.e(tag, "onActivityResult Error [ " + e.getMessage() + " ]");
    	}
    	
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
}
