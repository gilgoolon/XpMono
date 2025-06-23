import React, { useState, useEffect } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { Box, Button } from '@mui/material';
import ClientDetails from '../components/ClientDetails';
import axios from 'axios';
import { socket } from '../socket.js'

export default function ClientDetailsPage() {
  const { liverId } = useParams();
  const navigate = useNavigate();
  const [client, setClient] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    const handleConnect = () => {
      console.log('Connected to server');
      socket.emit('register', { client_id: liverId });
    };

    const handleDisconnect = () => {
      socket.emit('unregister', { client_id: liverId });
      console.log('Disconnected from server');
    };

    const fetchClientDetails = async () => {
      setIsLoading(true);
      try {
        const response = await axios.get(`${liverUrl(liverId)}`);
        setClient(response.data);
        setError(null);
      } catch (error) {
        console.error('Error fetching client details:', error);
        setError(error.message);
      } finally {
        setIsLoading(false);
      }
    };

    socket.on('connect', handleConnect);
    socket.on('disconnect', handleDisconnect);

    fetchClientDetails();
    socket.connect()

    return () => {
      socket.off('connect', handleConnect);
      socket.off('disconnect', handleDisconnect);
    };
  }, [liverId]);

  const handleSendCommand = async (commandData) => {
    const command = {
      command_data: commandData
    };
    
    return await axios.post(`${liverUrl(liverId)}/send-command`, command);
  };

  if (isLoading) {
    return <div>Loading...</div>;
  }

  if (error) {
    return (
      <Box sx={{ p: 3 }}>
        <div>Error: {error}</div>
        <Button variant="contained" onClick={() => navigate('/')} sx={{ mt: 2 }}>
          Back to List
        </Button>
      </Box>
    );
  }

  return (
    <Box>
      <Button 
        variant="outlined" 
        onClick={() => navigate('/')}
        sx={{ m: 2 }}
      >
        Back to List
      </Button>
      <ClientDetails client={client} onSendCommand={(data) => handleSendCommand(data)} />
    </Box>
  );
}
