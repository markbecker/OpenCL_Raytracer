package frontend;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import javax.swing.JOptionPane;

public class RaytracerRun implements Runnable
{
	private String commandLine;
	private String runName;
	private String outFile;
	private int width;
	private int height;
	OptionsPanel parent;
	StringBuilder outputText;
	
	public RaytracerRun(OptionsPanel parent, String commandLine, String runName, String outFile, int width, int height)
	{
		this.commandLine = commandLine;
		this.runName = runName;
		this.outFile = outFile;
		this.width = width;
		this.height = height;
		outputText = new StringBuilder();
		this.parent = parent;
	}
	
	public void run() {
		outputText.append(commandLine + "\r\n");		
		Runtime rt = Runtime.getRuntime();
		try {
			Process pr = rt.exec(commandLine);
			
			BufferedReader input = new BufferedReader(new InputStreamReader(pr.getErrorStream()));
			
			String line = null;
			
			while((line = input.readLine()) != null) {
				outputText.append(line + "\r\n");
			}
		} catch (IOException e) {
			JOptionPane.showMessageDialog(parent, "Raytracer executable not found.", "Run Error", JOptionPane.DEFAULT_OPTION);
			parent.unlockGUI();
			return;
		}
		if (outputText.toString().contains("failed"))
		{
			JOptionPane.showMessageDialog(parent, outputText.toString(), "Run Error", JOptionPane.DEFAULT_OPTION);
			parent.unlockGUI();
		}
		else
		{
			OutputFrame f = new OutputFrame(outputText.toString(), runName, outFile, width, height);
			f.pack();
			f.setVisible(true);
			parent.unlockGUI();
		}
	}

}
