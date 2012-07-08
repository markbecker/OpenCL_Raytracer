package frontend;
import javax.swing.JFrame;

public class RaytracerFrontend {

	private static void createGUI()
	{
		// Create main window
		JFrame mainFrame = new JFrame("OpenCL Raytracer GUI");
		mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		OptionsPanel oPanel = new OptionsPanel();
		
		mainFrame.add(oPanel);
		
		mainFrame.pack();
		mainFrame.setVisible(true);
	}
	
	public static void main(String[] args) 
	{
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				createGUI();
			}
		});
	}
}
