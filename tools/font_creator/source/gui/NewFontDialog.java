
package gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GraphicsEnvironment;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import util.Font;

/**
 * @author Fabian Maximilian Thiele
 */
public class NewFontDialog extends JDialog implements ActionListener,
		ChangeListener {
	private static final long serialVersionUID = 1893616638769058347L;
	public static final int APPROVE_OPTION = 1;
	public static final int CANCEL_OPTION = 2;

	private int returnValue;

	private JTextField fontNameTxt;
	private JSpinner fontWidthTxt;
	private JSpinner fontInitWidthTxt;
	private JSpinner fontHeightTxt;
	private JSpinner startIdxTxt;
	private JSpinner charCountTxt;
	private JSpinner charSpaceTxt;
	private JComboBox varColors;

	private JComboBox fontChooser;
	private JSpinner sizeTxt;
	private JCheckBox bold;
	private JCheckBox ital;

	private Font font;

	private String[] colorChoices = { "2 Colors (1 Bit per Pixel)",
			"4 Colors (2 Bit per Pixel)", "8 Colors (3 Bit per Pixel)",
			"256 Colors (8 Bit fixed)" };

	public NewFontDialog(MainWindow gui) {
		super(gui, "New Font", true);
		initGui();
		initDialog();
	}

	private void initGui() {
		this.setLayout(null);

		JPanel generalSettings = new JPanel(null);
		generalSettings.setBorder(BorderFactory.createTitledBorder("General"));
		JLabel fontNameL = new JLabel("Name:");
		fontNameL.setLocation(6, 19);
		fontNameL.setSize(95, 16);
		generalSettings.add(fontNameL);

		fontNameTxt = new JTextField("new Font");
		fontNameTxt.setLocation(75, 17);
		fontNameTxt.setSize(80, 20);
		generalSettings.add(fontNameTxt);

		JLabel fontWidthL = new JLabel("Width:");
		fontWidthL.setLocation(6, 42);
		fontWidthL.setSize(95, 16);
		generalSettings.add(fontWidthL);

		fontWidthTxt = new JSpinner();
		fontWidthTxt.setValue(new Integer(10));
		fontWidthTxt.setLocation(103, 40);
		fontWidthTxt.setSize(50, 20);
		generalSettings.add(fontWidthTxt);

		JLabel fontInitWidthL = new JLabel("Init Width:");
		fontInitWidthL.setLocation(6, 65);
		fontInitWidthL.setSize(95, 16);
		generalSettings.add(fontInitWidthL);

		fontInitWidthTxt = new JSpinner();
		fontInitWidthTxt.setValue(new Integer(10));
		fontInitWidthTxt.setLocation(103, 63);
		fontInitWidthTxt.setSize(50, 20);
		generalSettings.add(fontInitWidthTxt);

		JLabel fontHeightL = new JLabel("Height:");
		fontHeightL.setLocation(6, 88);
		fontHeightL.setSize(95, 16);
		generalSettings.add(fontHeightL);

		fontHeightTxt = new JSpinner();
		fontHeightTxt.setValue(new Integer(16));
		fontHeightTxt.setLocation(103, 86);
		fontHeightTxt.setSize(50, 20);
		fontHeightTxt.setToolTipText("Height will be discarded, if a Windows Font is imported");
		generalSettings.add(fontHeightTxt);

		JLabel startIdxL = new JLabel("Start Index:");
		startIdxL.setLocation(6, 111);
		startIdxL.setSize(95, 16);
		generalSettings.add(startIdxL);

		startIdxTxt = new JSpinner();
		startIdxTxt.setValue(new Integer(32));
		startIdxTxt.setLocation(103, 109);
		startIdxTxt.setSize(50, 20);
		generalSettings.add(startIdxTxt);

		JLabel charCountL = new JLabel("Char Count:");
		charCountL.setLocation(6, 134);
		charCountL.setSize(95, 16);
		generalSettings.add(charCountL);

		charCountTxt = new JSpinner();
		charCountTxt.setValue(new Integer(96));
		charCountTxt.setLocation(103, 132);
		charCountTxt.setSize(50, 20);
		generalSettings.add(charCountTxt);

		JLabel charSpaceL = new JLabel("Char Spacing:");
		charSpaceL.setLocation(6, 157);
		charSpaceL.setSize(95, 16);
		generalSettings.add(charSpaceL);

		charSpaceTxt = new JSpinner();
		charSpaceTxt.setValue(new Integer(1));
		charSpaceTxt.setLocation(103, 155);
		charSpaceTxt.setSize(50, 20);
		generalSettings.add(charSpaceTxt);

		generalSettings.setLocation(5, 5);
		generalSettings.setSize(161, 181);
		this.add(generalSettings);

		JPanel colorPanel = new JPanel(null);
		colorPanel.setBorder(BorderFactory.createTitledBorder("Colors"));

		varColors = new JComboBox(colorChoices);
		varColors.setLocation(6, 17);
		varColors.setSize(183, 22);
		colorPanel.add(varColors);

		colorPanel.setLocation(173, 5);
		colorPanel.setSize(195, 45);
		this.add(colorPanel);
		
		JPanel fontPanel = new JPanel(null);
		fontPanel.setBorder(BorderFactory.createTitledBorder("Import Font"));

		GraphicsEnvironment graEnv = GraphicsEnvironment.getLocalGraphicsEnvironment();
		String[] fontNames = graEnv.getAvailableFontFamilyNames();
		String[] choices = new String[fontNames.length + 1];
		choices[0] = "None";
		System.arraycopy(fontNames, 0, choices, 1, fontNames.length);
		fontChooser = new JComboBox(choices);
		fontChooser.setActionCommand("repaint");
		fontChooser.addActionListener(this);
		fontChooser.setLocation(6, 17);
		fontChooser.setSize(183, 22);
		fontPanel.add(fontChooser);

		JLabel sizeL = new JLabel("Size:");
		sizeL.setLocation(6, 46);
		sizeL.setSize(40, 16);
		fontPanel.add(sizeL);

		sizeTxt = new JSpinner();
		sizeTxt.addChangeListener(this);
		sizeTxt.setValue(new Integer(12));
		sizeTxt.setLocation(46, 44);
		sizeTxt.setSize(50, 20);
		fontPanel.add(sizeTxt);

		JLabel styleL = new JLabel("Style:");
		styleL.setLocation(6, 69);
		styleL.setSize(40, 16);
		fontPanel.add(styleL);

		bold = new JCheckBox("Bold");
		bold.setActionCommand("repaint");
		bold.addActionListener(this);
		bold.setLocation(46, 69);
		bold.setSize(50, 16);
		fontPanel.add(bold);

		ital = new JCheckBox("Italic");
		ital.setActionCommand("repaint");
		ital.addActionListener(this);
		ital.setLocation(96, 69);
		ital.setSize(60, 16);
		fontPanel.add(ital);

		fontPanel.setLocation(173, 55);
		fontPanel.setSize(195, 131);
		this.add(fontPanel);

		JButton ok = new JButton("Ok");
		ok.setActionCommand("ok");
		ok.addActionListener(this);
		ok.setLocation(112, 195);
		ok.setSize(75, 25);
		this.add(ok);

		JButton cancel = new JButton("Cancel");
		cancel.setActionCommand("cancel");
		cancel.addActionListener(this);
		cancel.setLocation(193, 195);
		cancel.setSize(75, 25);
		this.add(cancel);
	}
	
	private void initDialog() {
		setSize(380, 252);
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
		setLocation(d.width / 2 - getWidth() / 2,
				d.height / 2 - getHeight() / 2);
		setResizable(false);
	}

	public void paint(Graphics g) {
		super.paint(g);

		g.setColor(new Color(184, 207, 229));
		g.drawRect(182, 166, 182, 35);

		g.setColor(Color.WHITE);
		g.fillRect(183, 167, 181, 34);

		g.setColor(Color.BLACK);
		if (!fontChooser.getSelectedItem().equals("None")) {
			int style = java.awt.Font.PLAIN;
			if (bold.isSelected() || ital.isSelected()) {
				style = 0;
				if (bold.isSelected())
					style = java.awt.Font.BOLD;
				if (ital.isSelected())
					style |= java.awt.Font.ITALIC;
			}
			g.setFont(new java.awt.Font((String) fontChooser.getSelectedItem(),
					style, ((Integer) sizeTxt.getValue()).intValue()));
			g.setClip(183, 167, 181, 34);
			g.drawString("aA bB cC dD 1234", 185, 198);
		}
	}

	public void actionPerformed(ActionEvent e) {
		String cmd = e.getActionCommand();
		if (cmd.equals("repaint")) {
			repaint();
		} else if (cmd.equals("ok")) {
			try {
				returnValue = APPROVE_OPTION;

				String name = fontNameTxt.getText();
				int width = ((Integer) fontWidthTxt.getValue()).intValue();
				int initWidth = ((Integer) fontInitWidthTxt.getValue()).intValue();
				int height = ((Integer) fontHeightTxt.getValue()).intValue();
				int startIdx = ((Integer) startIdxTxt.getValue()).intValue();
				int charCount = ((Integer) charCountTxt.getValue()).intValue();
				int charSpacing = ((Integer) charSpaceTxt.getValue()).intValue();

				int bits = 1;

				if (varColors.getSelectedItem().equals(colorChoices[0])) {
					bits = 1;
				} else if (varColors.getSelectedItem().equals(colorChoices[1])) {
					bits = 2;
				} else if (varColors.getSelectedItem().equals(colorChoices[2])) {
					bits = 3;
				} else if (varColors.getSelectedItem().equals(colorChoices[3])) {
					bits = 8;
				}
				
				java.awt.Font importFont = null;
				if (!fontChooser.getSelectedItem().equals("None")) {
					int style = java.awt.Font.PLAIN;
					if (bold.isSelected() || ital.isSelected()) {
						style = 0;
						if (bold.isSelected())
							style = java.awt.Font.BOLD;
						if (ital.isSelected())
							style |= java.awt.Font.ITALIC;
					}
					importFont = new java.awt.Font(
							(String) fontChooser.getSelectedItem(), style,
							((Integer) sizeTxt.getValue()).intValue());
				}

				font = new Font(width, initWidth, height, startIdx, charCount,
						name, bits, importFont);
				font.setHorizontalSpace(charSpacing);

				dispose();
			} catch (Exception ex) {
				// ignore
			}
		} else if (cmd.equals("cancel")) {
			returnValue = CANCEL_OPTION;
			dispose();
		}
	}

	public int showNewFontDialog() {
		setVisible(true);

		return returnValue;
	}

	public Font getNewFont() {
		return font;
	}
	
	public void propertyChange(PropertyChangeEvent arg0) {
		repaint();
	}

	public void stateChanged(ChangeEvent arg0) {
		repaint();
	}
}
