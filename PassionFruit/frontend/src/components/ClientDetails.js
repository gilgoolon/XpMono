import React, { useState } from 'react';
import {
  Box, Typography, Paper, Grid, Card, CardContent,
  IconButton, Collapse, List, ListItem, ListItemText,
  TextField, Button, Alert, Snackbar, Dialog, DialogContent
} from '@mui/material';
import { LoadingButton } from '@mui/lab';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';
import ExpandLessIcon from '@mui/icons-material/ExpandLess';
import SendIcon from '@mui/icons-material/Send';
import ProductViewer from './ProductViewer';

export default function ClientDetails({ client, onSendCommand }) {
  const [commandData, setCommandData] = useState('');
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);
  const [selectedProduct, setSelectedProduct] = useState(null);
  const [ipHistoryExpanded, setIpHistoryExpanded] = useState(false);

  const handleSendCommand = async () => {
    setIsLoading(true);
    try {
      await onSendCommand(commandData);
      setCommandData('');
      setError(null);
    } catch (err) {
      setError(err.message);
    } finally {
      setIsLoading(false);
    }
  };

  const formatDate = (dateStr) => {
    try {
      const utcDate = new Date(dateStr + 'Z');
      if (isNaN(utcDate.getTime())) {
        return 'Invalid Date';
      }
      
      return new Intl.DateTimeFormat('en-GB', {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: false
      }).format(utcDate).replace(/\//g, '-');
    } catch (e) {
      console.error('Error formatting date:', e, dateStr);
      return 'Invalid Date';
    }
  };

  return (
    <Box sx={{ p: 3 }}>
      {/* Top Section - Client Overview */}
      <Paper elevation={2} sx={{ p: 3, mb: 3 }}>
        <Typography variant="h5" gutterBottom>
          Liver Details
        </Typography>
        <Grid container spacing={3}>
          <Grid item xs={12} md={4}>
            <Typography variant="subtitle2" color="text.secondary">
              Liver ID
            </Typography>
            <Typography variant="body1" gutterBottom>
              {client.client_id}
            </Typography>
          </Grid>
          <Grid item xs={12} md={4}>
            <Typography variant="subtitle2" color="text.secondary">
              Last Connection
            </Typography>
            <Typography variant="body1" gutterBottom>
              {formatDate(client.last_connection)}
            </Typography>
          </Grid>
          <Grid item xs={12} md={4}>
            <Box>
              <Typography variant="subtitle2" color="text.secondary">
                Current IP
              </Typography>
              <Box sx={{ display: 'flex', alignItems: 'center' }}>
                <Typography variant="body1">{client.current_ip || client.ip || client.ip_history?.[0]?.ip || 'N/A'}</Typography>
              </Box>
            </Box>
          </Grid>
        </Grid>
        
        {/* IP History Section */}
        <Box sx={{ mt: 2 }}>
          <Box
            sx={{
              display: 'flex',
              alignItems: 'center',
              cursor: 'pointer'
            }}
            onClick={() => setIpHistoryExpanded(!ipHistoryExpanded)}
          >
            <Typography variant="subtitle1">
              IP History
            </Typography>
            <IconButton size="small">
              {ipHistoryExpanded ? <ExpandLessIcon /> : <ExpandMoreIcon />}
            </IconButton>
          </Box>
          <Collapse in={ipHistoryExpanded}>
            <List dense>
              {client.ip_history.map((entry, index) => (
                <ListItem key={index}>
                  <ListItemText
                    primary={entry.ip}
                    secondary={formatDate(entry.last_seen)}
                  />
                </ListItem>
              ))}
            </List>
          </Collapse>
        </Box>
      </Paper>

      <Grid container spacing={3}>
        {/* Left Section - Products */}
        <Grid item xs={12} md={6}>
          <Paper elevation={2} sx={{ p: 3, height: '100%' }}>
            <Typography variant="h6" gutterBottom>
              Products
            </Typography>
            <Grid container spacing={2}>
              {client.products.map((productId, index) => {
                const product = client.parsed_products[productId];
                return (
                  <Grid item xs={12} sm={6} md={4} key={index}>
                    <Card 
                      sx={{ 
                        cursor: 'pointer',
                        '&:hover': {
                          backgroundColor: 'action.hover'
                        }
                      }}
                      onClick={() => setSelectedProduct(productId)}
                    >
                      <CardContent>
                        <Typography variant="subtitle2" noWrap>
                          {product?.formatted_type || 'Unknown Type'}
                        </Typography>
                        <Typography variant="body2" color="text.secondary" noWrap>
                          {product?.id || 'Unknown ID'}
                        </Typography>
                      </CardContent>
                    </Card>
                  </Grid>
                );
              })}
            </Grid>
          </Paper>
        </Grid>

        {/* Right Section - Command Interface */}
        <Grid item xs={12} md={6}>
          <Paper elevation={2} sx={{ p: 3, height: '100%' }}>
            <Typography variant="h6" gutterBottom>
              Send Command
            </Typography>
            <TextField
              fullWidth
              multiline
              rows={4}
              value={commandData}
              onChange={(e) => setCommandData(e.target.value)}
              placeholder="Enter command in JSON format"
              sx={{ mb: 2 }}
              error={!!error}
              helperText={error || 'Command will be sent in base64 format'}
            />
            <LoadingButton
              loading={isLoading}
              loadingPosition="start"
              startIcon={<SendIcon />}
              variant="contained"
              onClick={handleSendCommand}
              disabled={!commandData.trim()}
            >
              Send Command
            </LoadingButton>
          </Paper>
        </Grid>
      </Grid>

      {/* Product Viewer Dialog */}
      <Dialog
        open={!!selectedProduct}
        onClose={() => setSelectedProduct(null)}
        maxWidth="md"
        fullWidth
      >
        <DialogContent>
          {selectedProduct && (
            <Box>
              <Typography variant="h6" gutterBottom>
                {selectedProduct.split('/').pop()}
              </Typography>
              <ProductViewer 
                product={client.parsed_products[selectedProduct]}
                productPath={client.product_paths[selectedProduct]}
              />
            </Box>
          )}
        </DialogContent>
      </Dialog>
    </Box>
  );
}
