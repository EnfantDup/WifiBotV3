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
	public: delegate void Del(Sensors^ s);
	public:
	
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			mutex = gcnew System::Threading::Mutex;
			robot = gcnew Robot(mutex);
			m = gcnew Del(this, &Form1::setSensor);
		}

		void setSensor(Sensors^ s)
		{
			label3->Text = System::Convert::ToString(s->leftSpeed);
			label4->Text = System::Convert::ToString(s->rightSpeed);
			label5->Text = System::Convert::ToString(s->IR[0]);
			label6->Text = System::Convert::ToString(s->IR[1]);
			label7->Text = System::Convert::ToString(s->IR[2]);
			label8->Text = System::Convert::ToString(s->IR[3]);
		}

		void run()
		{
			/*Changer^ c = gcnew Changer(this, &Form1::setValue);
			int nb = 10;
			array<Object^>^myStringArray = {nb};
			this->Invoke(c, myStringArray);
			Sleep(2000);*/
			//GetLock^ l = gcnew GetLock(this, &Form::getLock);


			try
			{
				//Port et connexion
				Int32 port = 15020;
			    client = gcnew System::Net::Sockets::TcpClient(robot->getIp(), port);

				//Récupération du flux
				System::Net::Sockets::NetworkStream^ stream = client->GetStream();
				continuer = true;

				 
				while(client->Connected && continuer)
				{
					sendData(stream);
					receveData(stream);
				}

				if(client->Connected)
					client->Close();
		   }
		   catch ( ArgumentNullException^ e ) 
		   {
			  Console::WriteLine( "ArgumentNullException: {0}", e );
		   }
		   catch ( System::Net::Sockets::SocketException^ e ) 
		   {
			  Console::WriteLine( "SocketException: {0}", e );
		   }
		}
		void sendData(System::Net::Sockets::NetworkStream^ stream)
		{
			mutex->WaitOne();
			if(robot->getSimulateur())
			{
				//Simulateur

				robot->proceedSpeed();
				array<Byte>^ data = gcnew array<Byte>(2);
				char left = 0, right = 0;
				left = robot->getLeftSpeed()/4;
				right = robot->getRightSpeed()/4;
				mutex->ReleaseMutex();

				if(left >= 0)
					left = abs(left) + 64;
				else
					left = abs(left);

				if(right >= 0)
					right = abs(right) + 64;
				else
					right = abs(right);

				data[0] = left;
				data[1] = right;
				stream->Write(data, 0, data->Length);
			}
			else
			{
				//Robot
				char direction = 80; //En avant
				array<Byte>^ dataToSend = gcnew array<Byte>(9);

				robot->proceedSpeed();

				if(robot->getLeftSpeed() < 0)
					direction -= 16; //Roues gauche en arrière

				if(robot->getRightSpeed() < 0)
					direction -= 64; //Roues droites en arrière

				dataToSend[0] = (char)(255); //255
				dataToSend[1] = (char)(0x07); //Taille
				dataToSend[2] = (char)(abs(robot->getLeftSpeed())); //Vitesse à gauche
				dataToSend[3] = (char)(0);
				dataToSend[4] = (char)(abs(robot->getRightSpeed())); //Vitesse à droite
				dataToSend[5] = (char)(0);
				dataToSend[6] = (char)(direction);
				mutex->ReleaseMutex();

				//CRC
				short int crc = Crc16(dataToSend, 7);
				dataToSend[7] = (char)(crc);
				dataToSend[8] = (char)(crc >> 8);

				stream->Write(dataToSend, 0, dataToSend->Length);
			}
			
	}

		void receveData(System::Net::Sockets::NetworkStream^ stream)
		{
			mutex->WaitOne();
			if(robot->getSimulateur())
			{
				array<Byte>^ sensorData = gcnew array<Byte>(7);
				int av = stream->DataAvailable;
				Console::WriteLine("RCV:", av);
				while(stream->DataAvailable != 0)
				{
					stream->Read(sensorData, 0, sensorData->Length);
					Console::WriteLine("RCV:", sensorData[0]);
				}
			}
			else
			{
				array<Byte>^ sensorData = gcnew array<Byte>(21);
				while(stream->DataAvailable != 0)
				{
					int nbOctLu = stream->Read(sensorData, 0, sensorData->Length);
					Console::WriteLine("RCV:", nbOctLu);
				}
			
				/*int left = (int)((sensorData[1] << 8) + sensorData[0]);
				Del^ m = gcnew Del(this, &Form1::setSensor);
				this->Invoke(m, left);*/

				Sensors^ s = robot->getSensors();

				//Vitesse Gauche
				s->leftSpeed = (int)((sensorData[1] << 8) + sensorData[0]);
				if (s->leftSpeed > 32767) 
					s->leftSpeed -= 65536;

				//Niveau Batterie + Capteurs IR
				s->batterie = (int)(sensorData[2]);
				s->IR[0] = (int)(sensorData[3]);
				s->IR[1] = (int)(sensorData[4]);
				s->IR[2] = (int)(sensorData[11]);
				s->IR[3] = (int)(sensorData[12]);

				//Vitesse Droite
				s->rightSpeed = (int)((sensorData[10] << 8) + sensorData[9]);
				if (s->rightSpeed > 32767) 
					s->rightSpeed -= 65536;

				this->Invoke(m, s);
				
			}
			mutex->ReleaseMutex();
		}
		short int Crc16(array<Byte>^ Adresse_tab , unsigned char Taille_max)
		{
				unsigned int Crc = 0xFFFF;
				unsigned int Polynome = 0xA001;
				unsigned int CptOctet = 0;
				unsigned int CptBit = 0;
				unsigned int Parity= 0;

				for ( CptOctet= 1; CptOctet < Taille_max ; CptOctet++)
				{
					Crc ^= Adresse_tab[CptOctet];
					for ( CptBit = 0; CptBit <= 7 ; CptBit++)
					{
						Parity= Crc;
						Crc >>= 1;
						if (Parity%2 == true) 
							Crc ^= Polynome;
					}
				}
				return(Crc);
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

	private: Robot^ robot;
	private: bool continuer;
	private: Del^ m;
	

				
	private: System::Threading::Mutex^ mutex;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button4;

	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::Button^  button6;
	private: System::Threading::Thread^ myThread;

	private: System::Net::Sockets::TcpClient^ client;
private: System::Windows::Forms::TrackBar^  trackBar1;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label8;


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
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->BeginInit();
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
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(15, 195);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(35, 13);
			this->label2->TabIndex = 9;
			this->label2->Text = L"label2";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(317, 12);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(13, 13);
			this->label3->TabIndex = 10;
			this->label3->Text = L"0";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(317, 51);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(13, 13);
			this->label4->TabIndex = 11;
			this->label4->Text = L"0";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(317, 93);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(13, 13);
			this->label5->TabIndex = 12;
			this->label5->Text = L"0";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(317, 143);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(13, 13);
			this->label6->TabIndex = 13;
			this->label6->Text = L"0";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(317, 184);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(13, 13);
			this->label7->TabIndex = 14;
			this->label7->Text = L"0";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(317, 216);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(13, 13);
			this->label8->TabIndex = 15;
			this->label8->Text = L"0";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->ClientSize = System::Drawing::Size(405, 296);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
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
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {
			 mutex->WaitOne();
			 if(this->comboBox1->SelectedIndex == 0)
			 {
				 this->robot->setIp("127.0.0.1");
				 this->robot->setSimulateur(true);
			 }
			 else if(this->comboBox1->SelectedIndex == 1)
			 {
				 this->robot->setIp("192.168.1.106");
				this->robot->setSimulateur(false);
			 }
			 mutex->ReleaseMutex();

			 this->label1->ForeColor = System::Drawing::Color::Green;
			 //On desactive le bouton connexion et on active celui de deconnexion
			 this->button5->Enabled = false;
			 this->button6->Enabled = true;

			 myThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &Form1::run));
			 myThread->Start();
			
		 }

private: System::Void button6_Click(System::Object^  sender, System::EventArgs^  e) {
			 
			 this->label1->ForeColor = System::Drawing::Color::Red;
			 //On desactive le bouton connexion et on active celui de deconnexion
			 this->button6->Enabled = false;
			 this->button5->Enabled = true;
			 continuer = false;
		 }
private: System::Void Form1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->KeyCode == Keys::Z)
				 robot->setKeyUp(true);
			 if(e->KeyCode == Keys::S)
				 robot->setKeyDown(true);
			 if(e->KeyCode == Keys::Q)
				 robot->setKeyLeft(true);
			 if(e->KeyCode == Keys::D)
				 robot->setKeyRight(true);
			 mutex->ReleaseMutex();
		 }
private: System::Void Form1_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->KeyCode == Keys::Z)
				 robot->setKeyUp(false);
			 if(e->KeyCode == Keys::S)
				 robot->setKeyDown(false);
			 if(e->KeyCode == Keys::Q)
				 robot->setKeyLeft(false);
			 if(e->KeyCode == Keys::D)
				 robot->setKeyRight(false);
			 mutex->ReleaseMutex();
		 }
private: System::Void button1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 robot->setKeyUp(true);
			 mutex->ReleaseMutex();
		 }
private: System::Void button1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 robot->setKeyUp(false);
			 mutex->ReleaseMutex();
		 }
private: System::Void button2_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 robot->setKeyDown(true);
			 mutex->ReleaseMutex();
		 }
private: System::Void button2_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 robot->setKeyDown(false);
			 mutex->ReleaseMutex();
		 }
private: System::Void button3_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 robot->setKeyRight(true);
			 mutex->ReleaseMutex();
		 }
private: System::Void button3_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 robot->setKeyRight(false);
			 mutex->ReleaseMutex();
		 }
private: System::Void button4_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 robot->setKeyLeft(true);
			 mutex->ReleaseMutex();
		 }
private: System::Void button4_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 mutex->WaitOne();
			 if(e->Button == System::Windows::Forms::MouseButtons::Left)
				 robot->setKeyLeft(false);
			 mutex->ReleaseMutex();
		 }
private: System::Void trackBar1_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 mutex->WaitOne();
			 robot->setSpeed(this->trackBar1->Value);
			 mutex->ReleaseMutex();
		 }
private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}

