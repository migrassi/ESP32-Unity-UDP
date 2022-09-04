using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;

public class MoverUDP : MonoBehaviour
{

    public float speed = 2;
    public float salto = 0;
    Thread m_Thread;
    UdpClient m_Client;

    // Start is called before the first frame update
    void Start()
    {
        m_Thread = new Thread(new ThreadStart(ReceiveData));
        m_Thread.IsBackground = true;
        m_Thread.Start();
    }

    // Update is called once per frame
    void Update()
    {
        if (salto != 0)
        {
            //this.transform.Translate(Vector3.forward * salto);
            this.transform.Rotate(Vector3.up * salto);
            salto = 0;
        }



        if (Input.GetKey(KeyCode.UpArrow))
        {
            this.transform.Translate(Vector3.forward * Time.deltaTime);
            udpSend(transform.position);
        }

        if (Input.GetKey(KeyCode.DownArrow))
        {
            this.transform.Translate(Vector3.back * Time.deltaTime);
            udpSend(transform.position);
        }

        if (Input.GetKey(KeyCode.LeftArrow))
        {
            this.transform.Rotate(Vector3.up, -10);
            udpSend(new Vector3(transform.rotation.x, transform.rotation.y, transform.rotation.z));
        }

        if (Input.GetKey(KeyCode.RightArrow))
        {
            this.transform.Rotate(Vector3.up, 10);
            udpSend(new Vector3(transform.rotation.x, transform.rotation.y, transform.rotation.z));
        }
    }


    void ReceiveData()
    {

        try
        {

            m_Client = new UdpClient(1234);
            m_Client.EnableBroadcast = true;
            while (true)
            {

                IPEndPoint hostIP = new IPEndPoint(IPAddress.Any, 0);
                byte[] data = m_Client.Receive(ref hostIP);
                string returnData = Encoding.ASCII.GetString(data);
                Debug.Log(returnData);
                //salto = float.Parse(returnData);
                 if (!float.TryParse(returnData, System.Globalization.NumberStyles.Number,null,out float auxi)){
                    Debug.Log("Lo recibido no es un número");
                };
                salto = auxi;
            }
        }
        catch (Exception e)
        {
            Debug.Log(e);
            OnApplicationQuit();
        }
    }

    private void OnApplicationQuit()
    {
        if (m_Thread != null)
        {
            m_Thread.Abort();
        }

        if (m_Client != null)
        {
            m_Client.Close();
        }
    }
    void udpSend(Vector3 posx)
    {
        //var IP = IPAddress.Parse("192.168.0.74"); // Conecta al IP indicado (Modo Unicast) 
        var IP = IPAddress.Parse("239.1.2.3"); // Conecta al grupo Multicast indicado       
        int port = 1234;


        var udpClient1 = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        var sendEndPoint = new IPEndPoint(IP, port);


        try
        {

            //Sends a message to the host to which you have connected.
            //byte[] sendBytes = Encoding.ASCII.GetBytes("hello from unity");
            byte[] sendBytes = Encoding.ASCII.GetBytes(posx.ToString());
            udpClient1.SendTo(sendBytes, sendEndPoint);



        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }

    }
}


