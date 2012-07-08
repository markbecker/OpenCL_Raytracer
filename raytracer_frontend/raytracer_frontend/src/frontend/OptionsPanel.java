package frontend;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileNotFoundException;

import javax.swing.ButtonGroup;
import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JRadioButton;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

public class OptionsPanel extends JPanel
{
	private static final String programName = "raytracer";
	
	private JLabel imageSizeOptionsLabel, cameraOptionsLabel, generalOptionsLabel, platformOptionsLabel, kernelOptionsLabel, cameraXLabel, cameraYLabel, cameraZLabel,
					widthLabel, heightLabel, tracedepthLabel, wgSizeLabel, runNameLabel, filenameLabel, sceneLabel, viewportWLabel, viewportHLabel,
					viewportOptionsLabel;
	private JTextField cameraXField, cameraYField, cameraZField, widthField, heightField, tracedepthField, wgSizeField, runNameField, filenameField,
						sceneField, viewportWField, viewportHField;
	private JCheckBox useCPUBox, fastNormalizeBox, builtinNormalizeBox, nativeSqrtBox, builtinDotBox, builtinLenBox;
	private ButtonGroup radioGroup;
	private JRadioButton noOpenCLRadio, useAMDPlatRadio, useIntelPlatRadio;
	
	private JButton executeRunButton, quitButton, scnButton;
	
	public OptionsPanel()
	{
		super();
		
		GroupLayout layout = new GroupLayout(this);
		setLayout(layout);
		
		layout.setAutoCreateGaps(true);
		layout.setAutoCreateContainerGaps(true);
		
		// buttons
		executeRunButton = new JButton("Execute Run");
		quitButton = new JButton("Quit");
		scnButton = new JButton("Edit Scene");
		
		// button listeners
		executeRunButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					doRun();
				}		
		});
		
		quitButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				System.exit(0);
			}		
		});	
		
		scnButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				editScene();
			}		
		});		
		

		// labels
		imageSizeOptionsLabel = new JLabel(" Image Size Options ");
		cameraOptionsLabel = new JLabel("Camera Options");
		generalOptionsLabel = new JLabel("General Options");
		platformOptionsLabel = new JLabel("Platform Options");
		kernelOptionsLabel = new JLabel("Kernel Options");
		cameraXLabel = new JLabel("Camera X");
		cameraYLabel = new JLabel("Camera Y");
		cameraZLabel = new JLabel("Camera Z");
		widthLabel = new JLabel("Width");
		heightLabel = new JLabel("Height");
		tracedepthLabel = new JLabel("Trace Depth");
		wgSizeLabel = new JLabel("Workgroup Size");
		runNameLabel = new JLabel("Run Name");
		filenameLabel = new JLabel("Output File Name");
		viewportOptionsLabel = new JLabel("Viewport Options");
		viewportWLabel = new JLabel("Viewport Width");
		viewportHLabel = new JLabel("Viewport Height");
		sceneLabel = new JLabel("Scene File Name");
				
		// text fields
		widthField = new JTextField(5);
		widthField.setText("800");
		heightField = new JTextField(5);
		heightField.setText("600");
		cameraXField = new JTextField(3);
		cameraXField.setText("0.0");
		cameraYField = new JTextField(3);
		cameraYField.setText("0.25");
		cameraZField = new JTextField(3);
		cameraZField.setText("-7.0");
		tracedepthField = new JTextField(3);
		tracedepthField.setText("5");
		wgSizeField = new JTextField(3);
		wgSizeField.setText("64");
		runNameField = new JTextField(8);
		runNameField.setText("Default Run");
		filenameField = new JTextField(8);
		filenameField.setText("out.bmp");
		viewportWField = new JTextField(3);
		viewportWField.setText("6.0");
		viewportHField = new JTextField(3);
		viewportHField.setText("4.5");
		sceneField = new JTextField(8);
		sceneField.setText("def.scn");
	
		// checkboxes	
		useCPUBox = new JCheckBox("Use CPU");
		fastNormalizeBox = new JCheckBox("Fast Normalize");		
		builtinNormalizeBox = new JCheckBox("Built-in Normalize");		
		builtinDotBox = new JCheckBox("Built-in Dot Product");		
		builtinLenBox = new JCheckBox("Built-in Length");		
		nativeSqrtBox = new JCheckBox("Native Sqrt");
		
		// radio buttons
		noOpenCLRadio = new JRadioButton("No OpenCL");
		noOpenCLRadio.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				useCPUBox.setSelected(false);
				fastNormalizeBox.setSelected(false);	
				builtinNormalizeBox.setSelected(false);	
				builtinDotBox.setSelected(false);
				builtinLenBox.setSelected(false);
				nativeSqrtBox.setSelected(false);
				useCPUBox.setEnabled(false);
				fastNormalizeBox.setEnabled(false);	
				builtinNormalizeBox.setEnabled(false);	
				builtinDotBox.setEnabled(false);
				builtinLenBox.setEnabled(false);
				nativeSqrtBox.setEnabled(false);
				
			}		
		});
		
		useAMDPlatRadio = new JRadioButton("Use AMD Platform  ");	
		useAMDPlatRadio.setSelected(true);	
		useAMDPlatRadio.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				useCPUBox.setSelected(false);
				useCPUBox.setEnabled(true);
				fastNormalizeBox.setEnabled(true);	
				builtinNormalizeBox.setEnabled(true);	
				builtinDotBox.setEnabled(true);
				builtinLenBox.setEnabled(true);
				nativeSqrtBox.setEnabled(true);
			}		
		});			
		
		useIntelPlatRadio = new JRadioButton("Use Intel Platform ");
		useIntelPlatRadio.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				useCPUBox.setSelected(false);
				useCPUBox.setEnabled(false);
				fastNormalizeBox.setEnabled(true);	
				builtinNormalizeBox.setEnabled(true);	
				builtinDotBox.setEnabled(true);
				builtinLenBox.setEnabled(true);
				nativeSqrtBox.setEnabled(true);
			}		
		});
		
		
		radioGroup = new ButtonGroup();
		radioGroup.add(noOpenCLRadio);
		radioGroup.add(useAMDPlatRadio);
		radioGroup.add(useIntelPlatRadio);
		
		layout.setHorizontalGroup(
			layout.createParallelGroup(GroupLayout.Alignment.CENTER)
				.addComponent(generalOptionsLabel)
				.addGroup(layout.createSequentialGroup()
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(runNameLabel)
						.addComponent(filenameLabel)
						.addComponent(sceneLabel)
					)
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(runNameField)
						.addComponent(filenameField)
						.addComponent(sceneField)
					)
				)
				.addComponent(scnButton)
				.addComponent(platformOptionsLabel)
				.addGroup(layout.createSequentialGroup()
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addGroup(layout.createParallelGroup()
							.addComponent(useAMDPlatRadio)
							.addComponent(useIntelPlatRadio)
							.addComponent(noOpenCLRadio)
						)
					)					
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addGroup(layout.createParallelGroup()
							.addComponent(useCPUBox)
						)							
					)					
				)
				.addComponent(imageSizeOptionsLabel)
				.addGroup(layout.createSequentialGroup()
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(widthLabel)
						.addComponent(heightLabel)
					)
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(widthField)
						.addComponent(heightField)
					)
				)
				.addComponent(viewportOptionsLabel)
				.addGroup(layout.createSequentialGroup()
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(viewportWLabel)
						.addComponent(viewportHLabel)
					)
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(viewportWField)
						.addComponent(viewportHField)
					)
				)
				.addComponent(cameraOptionsLabel)
				.addGroup(layout.createSequentialGroup()
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(cameraXLabel)
						.addComponent(cameraYLabel)
						.addComponent(cameraZLabel)
					)
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(cameraXField)
						.addComponent(cameraYField)
						.addComponent(cameraZField)
					)
				)
				.addComponent(kernelOptionsLabel)
				.addGroup(layout.createSequentialGroup()
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(tracedepthLabel)
						.addComponent(wgSizeLabel)
					)
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(tracedepthField)
						.addComponent(wgSizeField)
					)
				)
				.addGroup(layout.createSequentialGroup()
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(builtinNormalizeBox)
						.addComponent(fastNormalizeBox)
						.addComponent(builtinLenBox)
					)
					.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(builtinDotBox)
						.addComponent(nativeSqrtBox)
					)
				)
				.addGroup(layout.createSequentialGroup()
					.addComponent(executeRunButton)
					.addComponent(quitButton)
				)
			);
		
		layout.setVerticalGroup(
			layout.createSequentialGroup()
				.addComponent(generalOptionsLabel)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(runNameLabel)
					.addComponent(runNameField)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(filenameLabel)
					.addComponent(filenameField)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(sceneLabel)
					.addComponent(sceneField)
				)
				.addComponent(scnButton)
				.addComponent(platformOptionsLabel)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(noOpenCLRadio)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(useIntelPlatRadio)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(useAMDPlatRadio)
					.addComponent(useCPUBox)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
				)
				.addComponent(imageSizeOptionsLabel)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(widthLabel)
					.addComponent(widthField)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(heightLabel)
					.addComponent(heightField)
				)
				.addComponent(viewportOptionsLabel)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(viewportWLabel)
					.addComponent(viewportWField)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(viewportHLabel)
					.addComponent(viewportHField)
				)
				.addComponent(cameraOptionsLabel)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(cameraXLabel)
					.addComponent(cameraXField)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(cameraYLabel)
					.addComponent(cameraYField)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(cameraZLabel)
					.addComponent(cameraZField)
				)
				.addComponent(kernelOptionsLabel)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(tracedepthLabel)
					.addComponent(tracedepthField)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(wgSizeLabel)
					.addComponent(wgSizeField)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(builtinNormalizeBox)
					.addComponent(builtinDotBox)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(fastNormalizeBox)
					.addComponent(nativeSqrtBox)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(builtinLenBox)
				)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(executeRunButton)
					.addComponent(quitButton)
				)
			);
		
		layout.linkSize(SwingConstants.HORIZONTAL, filenameLabel, widthLabel);
		layout.linkSize(SwingConstants.HORIZONTAL, filenameLabel, cameraXLabel);
		layout.linkSize(SwingConstants.HORIZONTAL, filenameLabel, tracedepthLabel);
		layout.linkSize(SwingConstants.HORIZONTAL, filenameLabel, viewportWLabel);
	}
	
	private void doRun()
	{
		StringBuilder executeString = new StringBuilder();
		int width, height;
		executeString.append(programName + " ");
		if (noOpenCLRadio.isSelected()){
			executeString.append("--no-opencl ");
		}else if(useIntelPlatRadio.isSelected()){
			executeString.append("--opencl-platform=0 ");
			executeString.append("--use-cpu ");
		}else{
			executeString.append("--opencl-platform=1 ");
		}
		if (useCPUBox.isSelected())
			executeString.append("--use-cpu ");
		try {
			executeString.append("--tracedepth=" + Integer.parseInt(tracedepthField.getText()) + " ");
		}
		catch(NumberFormatException e)
		{
			showError("Trace Depth is invalid. Please correct.");
			return;
		}
		try {
			executeString.append("--workgroup-size=" + Integer.parseInt(wgSizeField.getText()) + " ");
		}
		catch(NumberFormatException e)
		{
			showError("Workgroup Size is invalid. Please correct.");
			return;
		}
		if (fastNormalizeBox.isSelected())
			executeString.append("--fast-normalize ");
		if (builtinNormalizeBox.isSelected())
			executeString.append("--builtin-normalize ");
		if (nativeSqrtBox.isSelected())
			executeString.append("--native-sqrt ");
		if (builtinDotBox.isSelected())
			executeString.append("--builtin-dot ");
		if (builtinLenBox.isSelected())
			executeString.append("--builtin-length ");
		try {
			width = Integer.parseInt(widthField.getText());
			executeString.append("--width=" + Integer.parseInt(widthField.getText()) + " ");
		}
		catch (NumberFormatException e)
		{
			showError("Width is invalid. Please correct.");
			return;
		}
		try {
			height = Integer.parseInt(heightField.getText());
			executeString.append("--height=" + Integer.parseInt(heightField.getText()) + " ");
		}
		catch (NumberFormatException e)
		{
			showError("Height is invalid. Please correct.");
			return;
		}
		try {
			executeString.append("--camera-x=" + Float.parseFloat(cameraXField.getText()) + " ");
		}
		catch (NumberFormatException e)
		{
			showError("Camera X is invalid. Please correct.");
			return;
		}
		try {
			executeString.append("--camera-y=" + Float.parseFloat(cameraYField.getText()) + " ");
		}
		catch (NumberFormatException e)
		{
			showError("Camera Y is invalid. Please correct.");
			return;
		}
		try {
			executeString.append("--camera-z=" + Float.parseFloat(cameraZField.getText()) + " ");
		}
		catch (NumberFormatException e)
		{
			showError("Camera Z is invalid. Please correct.");
			return;
		}
		executeString.append("--outfile=" + filenameField.getText() + " ");
		executeString.append("--scene=" + sceneField.getText() + " ");
		try {
			executeString.append("--viewport-width=" + Float.parseFloat(viewportWField.getText()) + " ");
		}
		catch (NumberFormatException e)
		{
			showError("Viewport Width is invalid. Please correct.");
			return;
		}
		try {
			executeString.append("--viewport-height=" + Float.parseFloat(viewportHField.getText()) + " ");
		}
		catch (NumberFormatException e)
		{
			showError("Viewport Height is invalid. Please correct.");
			return;
		}
		
		// Do run
		RaytracerRun r = new RaytracerRun(this, executeString.toString(), runNameField.getText(), filenameField.getText(), width, height);
		Thread t = new Thread(r);
		
		lockGUI();
		
		t.start();		
	}
	
	public void lockGUI()
	{
		executeRunButton.setText("Running...");
		
		for (Component c : this.getComponents())
			c.setEnabled(false);		
	}
	
	public void unlockGUI()
	{
		executeRunButton.setText("Execute Run");
		
		for (Component c : this.getComponents())
			c.setEnabled(true);
	}
	
	private void showError(String e)
	{
		JOptionPane.showMessageDialog(this, e, "Run Error", JOptionPane.DEFAULT_OPTION);
	}
	
	private void editScene(){
		@SuppressWarnings("unused")
		SceneFrame sf = null;
		try {
			sf = new SceneFrame("Edit Scene", sceneField.getText());
		} catch (FileNotFoundException e) {
			System.err.println("Error: " + e.getMessage());
		}
	}
	
	private static final long serialVersionUID = 8763992587007323638L;
}
