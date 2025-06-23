import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  Box, Typography, Paper, Table, TableBody,
  TableCell, TableContainer, TableHead, TableRow,
  Skeleton
} from '@mui/material';
import { LoadingButton } from '@mui/lab';
import VisibilityIcon from '@mui/icons-material/Visibility';
import EditableField from '../components/EditableField';
import axios from 'axios'
import { API_BASE_URL } from "../Config.js";

function ClientNickname({ client }) {
  const [nickname, setNickname] = useState(client.nickname)

  const saveNickname = async (newName) => {
    try {
      await axios.post(`${API_BASE_URL}/api/set-nickname/${client.client_id}?nickname=${encodeURIComponent(newName)}`);
      setNickname(newName)
    } catch (error) {
      console.error('Failed to save:', error);
      alert('Error saving changes');
    }
  }

  return (
    <EditableField label="Nickname" value={nickname} onSave={(value) => saveNickname(value)} />
  );
}

export default function ClientList({ clients, isLoading, formatDate }) {
  const navigate = useNavigate();



  const LoadingTable = () => (
    <Box sx={{ width: '100%' }}>
      {[...Array(5)].map((_, index) => (
        <Box key={index} sx={{ display: 'flex', my: 2 }}>
          <Skeleton variant="rectangular" width="25%" height={40} sx={{ mr: 1 }} />
          <Skeleton variant="rectangular" width="25%" height={40} sx={{ mr: 1 }} />
          <Skeleton variant="rectangular" width="25%" height={40} sx={{ mr: 1 }} />
          <Skeleton variant="rectangular" width="20%" height={40} />
        </Box>
      ))}
    </Box>
  );

  if (isLoading) {
    return (
      <Paper sx={{ p: 2 }}>
        <LoadingTable />
      </Paper>
    );
  }

  return (
    <TableContainer component={Paper}>
      <Table>
        <TableHead>
          <TableRow>
            <TableCell>Liver ID</TableCell>
            <TableCell>Nickname</TableCell>
            <TableCell>Last Connection</TableCell>
            <TableCell>Current IP</TableCell>
            <TableCell>Actions</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {clients.length === 0 ? (
            <TableRow>
              <TableCell colSpan={4} align="center">
                <Typography variant="body1" color="text.secondary">
                  No clients found
                </Typography>
              </TableCell>
            </TableRow>
          ) : (
            clients.map((client) => (
              <TableRow key={client.client_id}>
                <TableCell>{client.client_id}</TableCell>
                <TableCell><ClientNickname client={client} /></TableCell>
                <TableCell>{formatDate(client.last_connection)}</TableCell>
                <TableCell>{client.current_ip}</TableCell>
                <TableCell>
                  <LoadingButton
                    variant="contained"
                    size="small"
                    onClick={() => navigate(`/liver/${client.client_id}`)}
                    startIcon={<VisibilityIcon />}
                  >
                    View Details
                  </LoadingButton>
                </TableCell>
              </TableRow>
            ))
          )}
        </TableBody>
      </Table>
    </TableContainer>
  );
}
