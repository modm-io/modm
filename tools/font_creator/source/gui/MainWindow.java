
package gui;

import gui.util.FontCreatorMenuBar;
import gui.util.FontCreatorSideToolBar;
import gui.util.FontCreatorTopToolBar;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.io.File;
import java.io.IOException;

import javax.swing.JDesktopPane;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JInternalFrame;
import javax.swing.JOptionPane;
import javax.swing.JTextField;

import util.Font;
import util.FontExport;
import util.SerializableFont;

/**
 * 
 * @author Fabian Maximilian Thiele
 * @author Fabian Greif
 */
public class MainWindow extends JFrame {
	private static final long serialVersionUID = 9045652199181844858L;
	private JDesktopPane desktop;
	private JTextField status;
	private int frameCount;

	private CharEditor selectedEditor;
	private FontCreatorSideToolBar sideToolbar;
	private FontCreatorTopToolBar topToolbar;

	public MainWindow() {
		frameCount = 0;

		initGUI();
		initFrame();
	}

	private void initGUI() {
		this.setLayout(new BorderLayout());

		FontCreatorMenuBar menubar = new FontCreatorMenuBar(this);
		setJMenuBar(menubar);

		topToolbar = new FontCreatorTopToolBar(this);
		this.add(BorderLayout.NORTH, topToolbar);

		sideToolbar = new FontCreatorSideToolBar(this);
		this.add(BorderLayout.WEST, sideToolbar);

		desktop = new JDesktopPane();
		desktop.setDragMode(JDesktopPane.OUTLINE_DRAG_MODE);
		this.add(BorderLayout.CENTER, desktop);

		status = new JTextField("ready");
		status.setEditable(false);
		this.add(BorderLayout.SOUTH, status);
	}

	private void initFrame() {
		setTitle("GLCD FontCreator 3.0");
		setSize(800, 600);
		
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
		setLocation(d.width / 2 - getWidth() / 2,
				d.height / 2 - getHeight() / 2);
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
	}

	public void newFont() {
		NewFontDialog dialog = new NewFontDialog(this);
		int retVal = dialog.showNewFontDialog();
		if (retVal == NewFontDialog.APPROVE_OPTION) {
			Font font = dialog.getNewFont();
			
			newFontEditor(font, "");
		}
	}

	private void newFontEditor(Font font, String filename) {
		FontEditor editor = new FontEditor(font, this, filename);
		editor.setLocation(frameCount * 30, frameCount * 30);
		if (++frameCount == 5)
			frameCount = 0;
		desktop.add(editor);
		editor.setVisible(true);
	}

	public void saveFont() {
		if (desktop.getSelectedFrame() instanceof FontEditor) {
			FontEditor editor = (FontEditor) desktop.getSelectedFrame();
			
			JFileChooser chooser = new JFileChooser(".");
			chooser.setSelectedFile(new File(editor.getFilename()));
			
			int rv = chooser.showSaveDialog(this);
			if (rv == JFileChooser.APPROVE_OPTION) {
				File file = chooser.getSelectedFile();
				try {
					SerializableFont font = new SerializableFont(
							editor.getEditedFont());
					font.writeToFile(file);
					editor.setFilename(file.getAbsolutePath());
				} catch (IOException e) {
					System.err.println("IOException: " + e.toString());
				}
			}
		}
	}

	public void loadFont() {
		JFileChooser chooser = new JFileChooser(".");
		int rv = chooser.showOpenDialog(this);
		if (rv == JFileChooser.APPROVE_OPTION) {
			File file = chooser.getSelectedFile();
			try {
				SerializableFont sFont = new SerializableFont(file);
				
				Font font = new Font(sFont);
				newFontEditor(font, file.getAbsolutePath());
			} catch (Exception e) {
				System.err.println("Excpetion: " + e.toString());
			}
		}
	}

	public void exportFont() {
		if (desktop.getSelectedFrame() instanceof FontEditor) {
			FontEditor editor = (FontEditor) desktop.getSelectedFrame();
			FontExport export = new FontExport(this);
			export.exportFont(editor.getEditedFont());
		}
	}

	public void editFontSettings() {
		if (desktop.getSelectedFrame() instanceof FontEditor) {
			FontEditor editor = (FontEditor) desktop.getSelectedFrame();
			Font font = editor.getEditedFont();
			String s = (String) JOptionPane.showInputDialog("Fontname:",
					font.getName());

			if ((s != null) && (s.length() > 0)) {
				font.setName(s);
				editor.setTitle(s);
			}
		}
	}

	public void selectedEditorChanged(CharEditor selectedEditor) {
		this.selectedEditor = selectedEditor;

		sideToolbar.setColor(selectedEditor.getColor(),
				selectedEditor.getEditorColorModel());
		topToolbar.setZoom((selectedEditor.getZoom()));
	}

	public void setActiveEditorColor(int color) {
		if (selectedEditor != null)
			selectedEditor.setColor(color);
	}

	public void setActiveEditorTool(int tool) {
		if (selectedEditor != null)
			selectedEditor.setTool(tool);
	}

	public void setActiveEditorZoom(int zoom) {
		JInternalFrame frame = desktop.getSelectedFrame();
		if (frame instanceof FontEditor) {
			FontEditor editor = (FontEditor) frame;
			editor.setZoom(zoom);
		}
	}

	public void setStatusText(String text) {
		status.setText(text);
	}
}
