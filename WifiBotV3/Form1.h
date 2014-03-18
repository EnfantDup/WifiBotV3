#pragma once

namespace WifiBotV3 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
		private: delegate void Changer(int nb);
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			client = new Client();
		}

		void run()
		{
			Changer^ c = gcnew Changer(this, &Form1::setValue);
			int nb = 10;
			array<Object^>^myStringArray = {nb};
			this->Invoke(c, myStringArray);
			Sleep(2000);
		}

		void setValue(int nb)
		{
			numericUpDown1->Value = nb;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	private: Client* client;
				

	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::TrackBar^  trackBar1;
	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::Button^  button6;
	private: System::Threading::Thread^ myThread;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown1;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(96, 12);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"HAUT";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button1_MouseDown);
			this->button1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button1_MouseUp);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(96, 70);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 1;
			this->button2->Text = L"BAS";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button2_MouseDown);
			this->button2->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button2_MouseUp);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(177, 41);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(75, 23);
			this->button3->TabIndex = 2;
			this->button3->Text = L"DROITE";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button3_MouseDown);
			this->button3->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button3_MouseUp);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(15, 41);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(75, 23);
			this->button4->TabIndex = 3;
			this->button4->Text = L"Gauche";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button4_MouseDown);
			this->button4->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button4_MouseUp);
			// 
			// trackBar1
			// 
			this->trackBar1->Location = System::Drawing::Point(15, 143);
			this->trackBar1->Maximum = 240;
			this->trackBar1->Minimum = 1;
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(237, 45);
			this->trackBar1->TabIndex = 4;
			this->trackBar1->TickFrequency = 10;
			this->trackBar1->Value = 200;
			this->trackBar1->ValueChanged += gcnew System::EventHandler(this, &Form1::trackBar1_ValueChanged);
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(142, 236);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(82, 23);
			this->button5->TabIndex = 5;
			this->button5->Text = L"Connexion";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &Form1::button5_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::Red;
			this->label1->Location = System::Drawing::Point(230, 241);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(32, 13);
			this->label1->TabIndex = 6;
			this->label1->Text = L"OK !";
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Local", L"Robot"});
			this->comboBox1->Location = System::Drawing::Point(15, 236);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(121, 21);
			this->comboBox1->TabIndex = 7;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox1_SelectedIndexChanged);
			this->comboBox1->SelectedIndex = 1;
			// 
			// button6
			// 
			this->button6->Enabled = false;
			this->button6->Location = System::Drawing::Point(142, 261);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(82, 23);
			this->button6->TabIndex = 8;
			this->button6->Text = L"Deconnexion";
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Click += gcnew System::EventHandler(this, &Form1::button6_Click);
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->Location = System::Drawing::Point(50, 195);
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(120, 20);
			this->numericUpDown1->TabIndex = 9;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->ClientSize = System::Drawing::Size(274, 296);
			this->Controls->Add(this->numericUpDown1);
			this->Controls->Add(this->button6);
			this->Controls->Add(this->comboBox1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->trackBar1);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->KeyPreview = true;
			this->Name = L"Form1";
			this->Text = L"WifiBot V3";
			this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::Form1_KeyUp);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::Form1_KeyDown);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {

			 if(this->comboBox1->SelectedIndex == 0)
			 {
				 this->client->setIp("127.0.0.1");
				 this->client->getRobot()->setSimulateur(true);
			 }
			 else if(this->comboBox1->SelectedIndex == 1)
			 {
				 this->client->setIp("192.168.1.106");
				this->client->getRobot()->setSimulateur(false);
			 }
			/*myThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &Form1::run));
			myThread->Start();*/

			 if(client->connexion())
			 {
				 this->label1->ForeColor = System::Drawing::Color::Green;
				 //On desactive le bouton connexion et on active celui de deconnexion
				 this->button5->Enabled = false;
				 this->button6->Enabled = true;
			 }
			 else
				 this->label1->ForeColor = System::Drawing::Color::Blue;
			
		 }
private: System::Void button6_Click(System::Object^  sender, System::EventArgs^  e) {
			 
			 client->deconnexion();
			 this->label1->ForeColor = System::Drawing::Color::Red;
			 //On desactive le bouton connexion et on active celui de deconnexion
			 this->button6->Enabled = false;
			 this->button5->Enabled = true;
		 }
private: System::Void Form1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if(e->KeyCode == Keys::Z)
				 client->getRobot()->setKeyUp(true);
			 if(e->KeyCode == Keys::S)
				 client->getRobot()->setKeyDown(true);
			 if(e->KeyCode == Keys::Q)
				 client->getRobot()->setKeyLeft(true);
			 if(e->KeyCode == Keys::D)
				 client->getRobot()->setKeyRight(true);
			 client->sendData();
		 }
private: System::Void Form1_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if(e->KeyCode == Keys::Z)
				 client->getRobot()->setKeyUp(false);
			 if(e->KeyCode == Keys::S)
				 client->getRobot()->setKeyDown(false);
			 if(e->KeyCode == Keys::Q)
				 client->getRobot()->setKeyLeft(false);
			 if(e->KeyCode == Keys::D)
				 client->getRobot()->setKeyRight(false);
			 client->sendData();
		 }
private: System::Void button1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 client->getRobot()->setKeyUp(true);
			 client->sendData();
		 }
private: System::Void button1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 client->getRobot()->setKeyUp(false);
			 client->sendData();
		 }
private: System::Void button2_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 client->getRobot()->setKeyDown(true);
			 client->sendData();
		 }
private: System::Void button2_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 client->getRobot()->setKeyDown(false);
			 client->sendData();
		 }
private: System::Void button3_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 client->getRobot()->setKeyRight(true);
			 client->sendData();
		 }
private: System::Void button3_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 client->getRobot()->setKeyRight(false);
			 client->sendData();
		 }
private: System::Void button4_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 client->getRobot()->setKeyLeft(true);
			 client->sendData();
		 }
private: System::Void button4_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 client->getRobot()->setKeyLeft(false);
			 client->sendData();
		 }
private: System::Void trackBar1_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 client->getRobot()->setSpeed(this->trackBar1->Value);
			 client->sendData();
		 }
private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}

