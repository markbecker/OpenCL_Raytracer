package frontend;


import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;

import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;

import java.text.DateFormat;
import java.text.SimpleDateFormat;

import java.util.ArrayList;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;

import static java.nio.file.StandardCopyOption.*;

public class SceneFrame extends JFrame {
	ArrayList<JTextField> tf = new ArrayList<JTextField>();
	JLabel lab;
	JPanel pane;
	JScrollPane outputScroller;
	JButton addBtn, commitBtn, closeBtn, delRow;
	GridBagConstraints c;
	FileInputStream fInStream;
	DataInputStream inStream;
	BufferedReader buffRead;
	BufferedWriter buffWrite;
	int gridyVal, fontSize, fontType, numOfCols, primCount;
	String fontFace, strLine, scnFile, colNames;
	String[] arrStr = null;

	public SceneFrame(String title, String fileName)
			throws FileNotFoundException {
		super(title);
		setSize(1200,900);
		scnFile = fileName;
		init();
		setColumnTitleArray();
		loadFileIntoArray();
		fillTable();
	}

	private void init() {
		gridyVal = 0;
		fontSize = 12;
		fontType = Font.PLAIN;
		fontFace = "Arial Narrow";
		numOfCols = 0;
		primCount = 0;	
		
		c = new GridBagConstraints();
		c.ipadx = 5;
		c.ipady = 3;
		c.anchor = GridBagConstraints.CENTER;
		c.fill = GridBagConstraints.HORIZONTAL;		
	}

	private void loadFileIntoArray() {
		try {
			fInStream = new FileInputStream(scnFile);
		} catch (FileNotFoundException e) {
			System.err.println("Error: " + e.getMessage());
		}
		inStream = new DataInputStream(fInStream);
		buffRead = new BufferedReader(new InputStreamReader(inStream));
		strLine = "";

		try {
			if ((strLine = buffRead.readLine()) != null) {
				primCount = Integer.parseInt(strLine);
				while ((strLine = buffRead.readLine()) != null) {
					arrStr = strLine.split(",");
					if (arrStr.length > 1) {
						for (int i = 0; i < numOfCols; i++) {
							if (i == numOfCols - 1)
								tf.add(new JTextField(arrStr[i].toString(), 14));
							else
								tf.add(new JTextField(arrStr[i].toString(), 4));
						}
					}
				}
			}
		} catch (IOException e) {
			System.err.println("Error: " + e.getMessage());
		}
		closeInStream();
	}

	private void closeInStream() {
		try {
			inStream.close();
		} catch (IOException e) {
			System.err.println("Error: " + e.getMessage());
		}
	}
	
	private void setColumnTitleArray() {
		colNames = "type,red,green,blue,reflection,refraction,refr_index,diff,spec,is_light,"
				+ "center_x/,center_y/,center_z/,radius/,prim name";
		arrStr = colNames.split(",");
		numOfCols = arrStr.length;
	}
	
	private void fillTable() {
		// remove old Scroller if exists
		if (outputScroller != null)
			remove(outputScroller);
		pane = new JPanel();
		pane.setLayout(new GridBagLayout());
		outputScroller = new JScrollPane(pane);
		outputScroller
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		add(outputScroller);
		gridyVal = 0;
		setTableHeader();
		setColumnTitles();

		int primNum = 1;
		int tfSize = tf.size();

		if (tfSize > 0) {
			for (int i = 0; i < primCount; i++) {

				lab = new JLabel("" + primNum + "");
				lab.setHorizontalAlignment(JLabel.CENTER);
				c.gridx = 0;
				c.gridy = gridyVal;
				pane.add(lab, c);

				for (int j = 0; j < numOfCols; j++) {
					int index = j + ((primNum - 1) * numOfCols);
					c.gridx = j + 1;
					c.gridy = gridyVal;
					pane.add(tf.get(index), c);
				}

				delRow = new JButton("Delete Prim: " + primNum);
				delRow.setMargin(new Insets(-3, -2, -3, -2));
				delRow.setActionCommand("" + primNum);
				delRow.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						deleteRow(arg0);
					}
				});
				c.gridx = numOfCols + 2;
				c.gridy = gridyVal;
				pane.add(delRow, c);

				gridyVal++;
				primNum++;
			}
			addButtons();
		}
		setLayout();
		pack();
		setSize(1100,650);
		setVisible(true);
	}
	
	private void setTableHeader() {
		lab = new JLabel(" ");
		lab.setHorizontalAlignment(JLabel.RIGHT);
		c.gridx = 15;
		c.gridy = gridyVal;
		pane.add(lab, c);

		gridyVal++;

		lab = new JLabel("File: ");
		lab.setHorizontalAlignment(JLabel.RIGHT);
		c.gridx = 0;
		c.gridy = gridyVal;
		pane.add(lab, c);

		lab = new JLabel("" + scnFile);
		c.gridwidth = 3; // 3 columns wide
		c.gridx = 1;
		c.gridy = gridyVal;
		pane.add(lab, c);
		c.gridwidth = 1; // 1 columns wide

		gridyVal++;
		
		lab = new JLabel("cnt: ");
		lab.setHorizontalAlignment(JLabel.RIGHT);
		c.gridx = 0;
		c.gridy = gridyVal;
		pane.add(lab, c);
		
		lab = new JLabel("" + primCount);
		lab.setHorizontalAlignment(JLabel.LEFT);
		c.gridx = 1;
		c.gridy = gridyVal;
		pane.add(lab, c);

		gridyVal++;
	}

	private void setColumnTitles(){
		arrStr = colNames.split(",");

		lab = new JLabel("Prim #");
		lab.setFont(new Font(fontFace, fontType, fontSize));
		lab.setHorizontalAlignment(JLabel.CENTER);
		c.gridx = 0;
		c.gridy = gridyVal;
		pane.add(lab, c);

		for (int i = 0; i < numOfCols; i++) {
			lab = new JLabel(arrStr[i].toString());
			lab.setFont(new Font(fontFace, fontType, fontSize));
			lab.setHorizontalAlignment(JLabel.CENTER);
			c.gridx = i + 1;
			c.gridy = gridyVal;
			pane.add(lab, c);
		}

		gridyVal++;

		c.gridy = gridyVal;
		lab = new JLabel("0=PLANE");
		lab.setFont(new Font(fontFace, fontType, fontSize));
		lab.setHorizontalAlignment(JLabel.CENTER);
		c.gridx = 1;
		pane.add(lab, c);

		lab = new JLabel("normal_x");
		lab.setFont(new Font(fontFace, fontType, fontSize));
		lab.setHorizontalAlignment(JLabel.CENTER);
		c.gridx = 11;
		pane.add(lab, c);

		lab = new JLabel("normal_y");
		lab.setFont(new Font(fontFace, fontType, fontSize));
		lab.setHorizontalAlignment(JLabel.CENTER);
		c.gridx = 12;
		pane.add(lab, c);

		lab = new JLabel("normal_z");
		lab.setFont(new Font(fontFace, fontType, fontSize));
		lab.setHorizontalAlignment(JLabel.CENTER);
		c.gridx = 13;
		pane.add(lab, c);

		lab = new JLabel("depth");
		lab.setFont(new Font(fontFace, fontType, fontSize));
		lab.setHorizontalAlignment(JLabel.CENTER);
		c.gridx = 14;
		pane.add(lab, c);

		gridyVal++;

		lab = new JLabel("1=SPHERE");
		lab.setFont(new Font(fontFace, fontType, fontSize));
		lab.setHorizontalAlignment(JLabel.CENTER);
		c.gridx = 1;
		c.gridy = gridyVal;
		pane.add(lab, c);

		gridyVal++;		
	}
	
	private void addButtons() {
		lab = new JLabel(" ");
		c.gridx = 0;
		c.gridy = gridyVal;
		pane.add(lab, c);

		gridyVal++;

		addBtn = new JButton("Add Primative");
		commitBtn = new JButton("Commit Changes");
		closeBtn = new JButton("Close");

		addBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				addPrim();
			}
		});

		commitBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				commitChanges();
			}
		});

		closeBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				setVisible(false);
			}
		});

		c.gridy = gridyVal;
		c.gridwidth = 3; // 3 columns wide
		c.gridx = 1;
		pane.add(addBtn, c);
		c.gridwidth = 3; // 3 columns wide
		c.gridx = 5;
		pane.add(commitBtn, c);
		c.gridwidth = 2; // 3 columns wide
		c.gridx = 9;
		pane.add(closeBtn, c);
		c.gridwidth = 1; // 3 columns wide

		gridyVal++;

		lab = new JLabel(" ");
		c.gridx = 0;
		c.gridy = gridyVal;
		pane.add(lab, c);
	}

	private void setLayout() {
		setLayout(new BoxLayout(getContentPane(), BoxLayout.PAGE_AXIS));
		setDefaultCloseOperation(javax.swing.JFrame.DISPOSE_ON_CLOSE);
	}

	private void addPrim() {
		for (int i = 0; i < numOfCols; i++) {
			if (i == numOfCols - 1)
				tf.add(new JTextField("no name", 14));
			else
				tf.add(new JTextField("0", 4));
		}
		primCount = tf.size() / numOfCols;
		fillTable();
		outputScroller.getVerticalScrollBar().setValue(outputScroller.getVerticalScrollBar().getMaximum());
	}

	private void commitChanges() {
		int response = JOptionPane
				.showConfirmDialog(null,
						"Would you like to make a\nback-up copy of the original scene file?");
		if (response == JOptionPane.YES_OPTION) {
			try {
				DateFormat formatter = new SimpleDateFormat(
						"yyyy.MM.dd.hh.mm.ss");
				Path pathFrom = FileSystems.getDefault().getPath(scnFile);
				Path pathTo = FileSystems.getDefault().getPath(
						formatter.format(System.currentTimeMillis()) + "."
								+ scnFile);
				Files.copy(pathFrom, pathTo, REPLACE_EXISTING);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		if (response != JOptionPane.CANCEL_OPTION) {
			buffWrite = null;
			int tfSize = tf.size();
			int primNum = (tfSize / numOfCols);
			try {
				buffWrite = new BufferedWriter(new FileWriter(scnFile));
				buffWrite.write("" + primNum);
				buffWrite.newLine();
				for (int i = 0; i < tfSize; i++) {
					buffWrite.write("" + tf.get(i).getText());
					if (((i + 1) % numOfCols) == 0)
						buffWrite.newLine();
					else
						buffWrite.write(",");
				}
			} catch (FileNotFoundException ex) {
				ex.printStackTrace();
			} catch (IOException ex) {
				ex.printStackTrace();
			} finally {
				try {
					if (buffWrite != null) {
						buffWrite.flush();
						buffWrite.close();
					}
				} catch (IOException ex) {
					ex.printStackTrace();
				}
			}
		}
	}

	private void deleteRow(ActionEvent arg0) {
		int response = JOptionPane.showConfirmDialog(null,
				"Sure you want to delete primative #" + arg0.getActionCommand() + "?");
		if (response == JOptionPane.YES_OPTION) {
			int primToRemove = Integer.parseInt(arg0.getActionCommand());
			gridyVal = primToRemove+5;
			// remove JLabel and JTextField from row
			int tfStartIndexToRemove = ((primToRemove-1) * numOfCols);
			int tfEndIndexToRemove = tfStartIndexToRemove+(numOfCols-1);
			tf.subList(tfStartIndexToRemove, tfEndIndexToRemove+1).clear();
			primCount = tf.size() / numOfCols;
			fillTable();
		}
	}

	private static final long serialVersionUID = -414120867649790907L;
}
