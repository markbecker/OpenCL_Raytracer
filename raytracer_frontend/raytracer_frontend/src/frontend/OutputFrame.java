package frontend;

import java.awt.Image;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

import external.BackgroundPanel;

public class OutputFrame extends JFrame
{
	JTextArea outputText;
	BackgroundPanel outImagePanel;
	JScrollPane outputScroller;

	
	public OutputFrame(String output, String title, String imageName, int width, int height)
	{
		super(title);
		outputText = new JTextArea();
		outputScroller = new JScrollPane(outputText);
		outputText.setEditable(false);
		outputScroller.setVerticalScrollBarPolicy(
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		outputText.setLineWrap(true);
		outputText.setWrapStyleWord(true);
		outputText.append(output);
		setLayout(new BoxLayout(getContentPane(), BoxLayout.PAGE_AXIS));
		setDefaultCloseOperation(javax.swing.JFrame.DISPOSE_ON_CLOSE);
		add(outputScroller);
		Image outImage;
		try {
			outImage = ImageIO.read(new File(imageName));
		} catch (IOException e) {
			outputText.append("Error: Unable to load output image.\r\n");
			return;
		}
		outImagePanel = new BackgroundPanel(outImage, BackgroundPanel.ACTUAL);
		add(outImagePanel);
	}
	
	private static final long serialVersionUID = 600535547312935062L;
}
