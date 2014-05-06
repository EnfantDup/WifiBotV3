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
			form = this;
		}

		void setSensor(Sensors^ s)
		{
			label3->Text = System::Convert::ToString(s->leftSpeed);
			label4->Text = System::Convert::ToString(s->rightSpeed);
			label5->Text = System::Convert::ToString(s->IR[0]);
			label6->Text = System::Convert::ToString(s->IR[1]);
			label7->Text = System::Convert::ToString(s->IR[2]);
			label8->Text = System::Convert::ToString(s->IR[3]);
			//label2->Text = System::Convert::ToString(s->batterie);
			label9->Text = System::Convert::ToString(s->batterie + "%");
			
			//progressBar1->Value = (int)(per);
			//Application::DoEvents();
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
					System::Threading::Thread::Sleep(5);
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
					direction -= 64; //Roues gauche en arrière

				if(robot->getRightSpeed() < 0)
					direction -= 16; //Roues droites en arrière

				dataToSend[0] = (char)(255); //255
				dataToSend[1] = (char)(0x07); //Taille
				dataToSend[2] = (char)(abs(robot->getLeftSpeed())); //Vitesse à gauche
				dataToSend[3] = (char)(abs(robot->getLeftSpeed()));
				dataToSend[4] = (char)(abs(robot->getRightSpeed())); //Vitesse à droite
				dataToSend[5] = (char)(abs(robot->getRightSpeed()));
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
				
				Sensors^ s = robot->getSensors();
				s->leftSpeed = sensorData[0];
				s->rightSpeed = sensorData[0];
				array<Object^>^ d = {s};
				this->BeginInvoke(m, d);
			}
			else
			{
				int nbOctLu;
				array<Byte>^ sensorData = gcnew array<Byte>(21);
				while(stream->DataAvailable != 0)
				{
					nbOctLu = stream->Read(sensorData, 0, sensorData->Length);
					Console::WriteLine("RCV:", nbOctLu);
				}
			
				/*int left = (int)((sensorData[1] << 8) + sensorData[0]);
				Del^ m = gcnew Del(this, &Form1::setSensor);
				this->Invoke(m, left);*/
				if(nbOctLu != 0)
				{
					//Nous avons reçu des données on met à jour
					Sensors^ s = robot->getSensors();
					s->oct = nbOctLu;
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

					this->BeginInvoke(m, s);
				}
				
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
	private: System::Windows::Forms::Form^ form;
	

				
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

private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::PictureBox^  pictureBox1;
private: System::Windows::Forms::WebBrowser^  webBrowser1;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label9;

private: System::Windows::Forms::Button^  button7;
private: System::Windows::Forms::Button^  button8;
private: System::Windows::Forms::Button^  button9;
private: System::Windows::Forms::Button^  button10;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Button^  button11;



private: System::ComponentModel::IContainer^  components;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->webBrowser1 = (gcnew System::Windows::Forms::WebBrowser());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->button7 = (gcnew System::Windows::Forms::Button());
			this->button8 = (gcnew System::Windows::Forms::Button());
			this->button9 = (gcnew System::Windows::Forms::Button());
			this->button10 = (gcnew System::Windows::Forms::Button());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->button11 = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
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
			this->button5->Location = System::Drawing::Point(148, 188);
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
			this->label1->Location = System::Drawing::Point(236, 193);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(32, 13);
			this->label1->TabIndex = 6;
			this->label1->Text = L"OK !";
			// 
			// comboBox1
			// 
			this->comboBox1->DisplayMember = L"Robot";
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Local", L"Robot"});
			this->comboBox1->Location = System::Drawing::Point(21, 188);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(121, 21);
			this->comboBox1->TabIndex = 0;
			this->comboBox1->ValueMember = L"Robot";
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox1_SelectedIndexChanged);
			// 
			// button6
			// 
			this->button6->Enabled = false;
			this->button6->Location = System::Drawing::Point(148, 213);
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
			this->trackBar1->Minimum = 10;
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(237, 45);
			this->trackBar1->TabIndex = 4;
			this->trackBar1->TickFrequency = 10;
			this->trackBar1->Value = 180;
			this->trackBar1->ValueChanged += gcnew System::EventHandler(this, &Form1::trackBar1_ValueChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(338, 181);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(13, 13);
			this->label3->TabIndex = 10;
			this->label3->Text = L"0";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(402, 181);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(13, 13);
			this->label4->TabIndex = 11;
			this->label4->Text = L"0";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(306, 51);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(13, 13);
			this->label5->TabIndex = 12;
			this->label5->Text = L"0";
			this->label5->Click += gcnew System::EventHandler(this, &Form1::label5_Click);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(431, 143);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(13, 13);
			this->label6->TabIndex = 13;
			this->label6->Text = L"0";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(431, 51);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(13, 13);
			this->label7->TabIndex = 14;
			this->label7->Text = L"0";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(306, 143);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(13, 13);
			this->label8->TabIndex = 15;
			this->label8->Text = L"0";
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(325, 41);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(100, 137);
			this->pictureBox1->TabIndex = 16;
			this->pictureBox1->TabStop = false;
			// 
			// webBrowser1
			// 
			this->webBrowser1->AllowNavigation = false;
			this->webBrowser1->IsWebBrowserContextMenuEnabled = false;
			this->webBrowser1->Location = System::Drawing::Point(483, 12);
			this->webBrowser1->MinimumSize = System::Drawing::Size(20, 20);
			this->webBrowser1->Name = L"webBrowser1";
			this->webBrowser1->Size = System::Drawing::Size(359, 257);
			this->webBrowser1->TabIndex = 17;
			this->webBrowser1->Url = (gcnew System::Uri(L"http://192.168.1.106:8080/javascript_simple.html", System::UriKind::Absolute));
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(322, 213);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(43, 13);
			this->label2->TabIndex = 18;
			this->label2->Text = L"Batterie";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(344, 236);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(13, 13);
			this->label9->TabIndex = 19;
			this->label9->Text = L"0";
			this->label9->Click += gcnew System::EventHandler(this, &Form1::label9_Click);
			// 
			// button7
			// 
			this->button7->Location = System::Drawing::Point(968, 103);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(75, 23);
			this->button7->TabIndex = 21;
			this->button7->Text = L"HAUT";
			this->button7->UseVisualStyleBackColor = true;
			this->button7->Click += gcnew System::EventHandler(this, &Form1::button7_Click);
			// 
			// button8
			// 
			this->button8->Location = System::Drawing::Point(887, 132);
			this->button8->Name = L"button8";
			this->button8->Size = System::Drawing::Size(75, 23);
			this->button8->TabIndex = 22;
			this->button8->Text = L"GAUCHE";
			this->button8->UseVisualStyleBackColor = true;
			this->button8->Click += gcnew System::EventHandler(this, &Form1::button8_Click);
			// 
			// button9
			// 
			this->button9->Location = System::Drawing::Point(1049, 132);
			this->button9->Name = L"button9";
			this->button9->Size = System::Drawing::Size(75, 23);
			this->button9->TabIndex = 23;
			this->button9->Text = L"DROITE";
			this->button9->UseVisualStyleBackColor = true;
			this->button9->Click += gcnew System::EventHandler(this, &Form1::button9_Click);
			// 
			// button10
			// 
			this->button10->Location = System::Drawing::Point(968, 161);
			this->button10->Name = L"button10";
			this->button10->Size = System::Drawing::Size(75, 23);
			this->button10->TabIndex = 24;
			this->button10->Text = L"BAS";
			this->button10->UseVisualStyleBackColor = true;
			this->button10->Click += gcnew System::EventHandler(this, &Form1::button10_Click);
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 25, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label10->ForeColor = System::Drawing::SystemColors::MenuHighlight;
			this->label10->Location = System::Drawing::Point(873, 12);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(276, 39);
			this->label10->TabIndex = 25;
			this->label10->Text = L"Contrôle Caméra";
			// 
			// button11
			// 
			this->button11->Location = System::Drawing::Point(968, 133);
			this->button11->Name = L"button11";
			this->button11->Size = System::Drawing::Size(75, 23);
			this->button11->TabIndex = 26;
			this->button11->Text = L"RESET";
			this->button11->UseVisualStyleBackColor = true;
			this->button11->Click += gcnew System::EventHandler(this, &Form1::button11_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->ClientSize = System::Drawing::Size(1161, 275);
			this->Controls->Add(this->button11);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->button10);
			this->Controls->Add(this->button9);
			this->Controls->Add(this->button8);
			this->Controls->Add(this->button7);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->webBrowser1);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
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
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
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
private: System::Void label5_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void button7_Click(System::Object^  sender, System::EventArgs^  e) {
			 //CAMERA HAUT
			 //System::Net::HttpWebRequest^ myReq = dynamic_cast<System::Net::HttpWebRequest^>(System::Net::WebRequest::Create("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=10094853&group=1&value=-200"));
			 System::Net::WebRequest^ request = System::Net::WebRequest::Create("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=10094853&group=1&value=-200");
			 System::Net::HttpWebResponse^ response = dynamic_cast<System::Net::HttpWebResponse^>(request->GetResponse());
			 response->Close();
		 }
private: System::Void button8_Click(System::Object^  sender, System::EventArgs^  e) {
			 //CAMERA GAUCHE
			 System::Net::WebRequest^ request = System::Net::WebRequest::Create("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=10094852&group=1&value=200");
			 System::Net::HttpWebResponse^ response = dynamic_cast<System::Net::HttpWebResponse^>(request->GetResponse());
			 response->Close();
		 }
private: System::Void button9_Click(System::Object^  sender, System::EventArgs^  e) {
			 //CAMERA DROITE
			 System::Net::WebRequest^ request = System::Net::WebRequest::Create("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=10094852&group=1&value=-200");
			 System::Net::HttpWebResponse^ response = dynamic_cast<System::Net::HttpWebResponse^>(request->GetResponse());
			 response->Close();
		 }
private: System::Void button10_Click(System::Object^  sender, System::EventArgs^  e) {
			 //CAMERA BAS
			 System::Net::WebRequest^ request = System::Net::WebRequest::Create("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=10094853&group=1&value=200");
			 System::Net::HttpWebResponse^ response = dynamic_cast<System::Net::HttpWebResponse^>(request->GetResponse());
			 response->Close();
		 }
private: System::Void button11_Click(System::Object^  sender, System::EventArgs^  e) {
			 System::Net::WebRequest^ request = System::Net::WebRequest::Create("http://192.168.1.106:8080/?action=command&dest=0&plugin=0&id=168062211&group=1&value=3");
			 System::Net::HttpWebResponse^ response = dynamic_cast<System::Net::HttpWebResponse^>(request->GetResponse());
			 response->Close();
		 }
private: System::Void label9_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}

