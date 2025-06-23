import React, { useState, useEffect } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { Box, Button } from '@mui/material';
import ClientDetails from '../components/ClientDetails';
import axios from 'axios';
import { API_BASE_URL } from "../Config.js";

export default function ClientDetailsPage() {
  const { liverId } = useParams();
  const navigate = useNavigate();
  const [client, setClient] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchClientDetails = async () => {
      setIsLoading(true);
      try {
        const response = await axios.get(`${API_BASE_URL}/api/clients/${liverId}`);
        setClient(response.data);
        setError(null);
      } catch (error) {
        console.error('Error fetching client details:', error);
        setError(error.message);
      } finally {
        setIsLoading(false);
      }
    };
    fetchClientDetails();
  }, [liverId]);

  const handleSendCommand = async (commandData) => {
    const payload = {
      client_id: liverId,
      data: commandData
    };
    
    return axios.post(`${API_BASE_URL}/api/commands`, payload);
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
      <ClientDetails client={client} onSendCommand={handleSendCommand} />
    </Box>
  );
}
