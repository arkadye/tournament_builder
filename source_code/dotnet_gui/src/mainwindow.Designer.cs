namespace tournament_builder_gui
{
    partial class mainwindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.GoAwayMsg = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // GoAwayMsg
            // 
            this.GoAwayMsg.AutoSize = true;
            this.GoAwayMsg.Location = new System.Drawing.Point(43, 39);
            this.GoAwayMsg.Name = "GoAwayMsg";
            this.GoAwayMsg.Size = new System.Drawing.Size(221, 39);
            this.GoAwayMsg.TabIndex = 0;
            this.GoAwayMsg.Text = "This GUI is a placeholder for now\r\nSome actual functionality may be added later.\r" +
    "\nFor now, you can use the command line.";
            // 
            // mainwindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(314, 126);
            this.Controls.Add(this.GoAwayMsg);
            this.Name = "mainwindow";
            this.Text = "mainwindow";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label GoAwayMsg;
    }
}