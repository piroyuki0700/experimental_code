using System;
using System.Net.Sockets;
using System.Text;
using System.Windows.Forms;

namespace UdpUnicodeSender
{
    public class MainForm : Form
    {
        private TextBox textBox;
        private Button sendButton;

        public MainForm()
        {
            // テキストボックスの初期設定
            textBox = new TextBox()
            {
                Location = new System.Drawing.Point(10, 10),
                Width = 300
            };

            // 送信ボタンの初期設定
            sendButton = new Button()
            {
                Location = new System.Drawing.Point(320, 10),
                Text = "送信"
            };
            sendButton.Click += SendButton_Click;

            // フォームにコントロールを追加
            this.Controls.Add(textBox);
            this.Controls.Add(sendButton);
            this.Text = "UDP Unicode Sender";
            this.StartPosition = FormStartPosition.CenterScreen;
            this.ClientSize = new System.Drawing.Size(420, 50);
        }

        private void SendButton_Click(object sender, EventArgs e)
        {
            try
            {
                // テキストボックスから文字列を取得
                string input = textBox.Text;

                // Unicode(UTF-16 LE)に変換（.NETのEncoding.UnicodeはUTF-16 LE）
                byte[] textBytes = Encoding.Unicode.GetBytes(input);

                // NUL終端（2バイト）を付加
                byte[] nulTerminator = new byte[2];  // 初期値は0で埋められているのでNUL

                // 送信データのサイズは「変換済み文字列バイト数 + NUL終端(2バイト)」
                int dataSize = textBytes.Length + nulTerminator.Length;
                byte[] sizeBytes = BitConverter.GetBytes(dataSize); // little-endian

                // 送信バッファ作成：先頭4バイトにサイズ、その後にテキスト、末尾にNUL終端
                byte[] sendBuffer = new byte[4 + dataSize];
                Buffer.BlockCopy(sizeBytes, 0, sendBuffer, 0, 4);
                Buffer.BlockCopy(textBytes, 0, sendBuffer, 4, textBytes.Length);
                Buffer.BlockCopy(nulTerminator, 0, sendBuffer, 4 + textBytes.Length, nulTerminator.Length);

                // UDPクライアントを使って送信
                using (UdpClient udpClient = new UdpClient())
                {
                    udpClient.Send(sendBuffer, sendBuffer.Length, "localhost", 5000);
                }

                MessageBox.Show("送信完了");
            }
            catch (Exception ex)
            {
                MessageBox.Show("エラー：" + ex.Message);
            }
        }

        [STAThread]
        public static void Main()
        {
            Application.EnableVisualStyles();
            Application.Run(new MainForm());
        }
    }
}

